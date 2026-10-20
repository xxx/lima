private object writer;
private int checks;
private int polls;
private int saw_partial;
private int saw_overlap;
private int growths;
private int previous_size;

int valid_load(string path, string func, object caller, string program) { return 1; }
int valid_read(string path, string func, object caller, string program) { return 1; }
int valid_write(string path, string func, object caller, string program) { return 1; }
int valid_inherit(string path, string from) { return 1; }
int valid_shutdown(object caller, string program) { return 1; }

void error_handler(mapping problem) {
    dump("TAR_ERROR: " + problem["diagnostic"] + "\n");
    efun::shutdown(1);
}

private void verify_tar(int passed, string label) {
    if (!passed) throw("TAR_FAIL: " + label);
    checks++;
    dump("TAR_OK: " + label + "\n");
}

private void refused(string source, string archive, string reason, string label) {
    mixed problem = catch(writer->tar(source, archive));
    verify_tar(stringp(problem) && strindex(problem, reason) >= 0, label);
}

private void wait_for_archives() {
    int size = file_size("/output/large.tar");
    if (size > previous_size) growths++;
    previous_size = size;
    if (size > 512 && size < LARGE_ARCHIVE_SIZE) {
        saw_partial = 1;
        if (file_size("/output/empty.tar") == 1536) saw_overlap = 1;
    }
    if (size == LARGE_ARCHIVE_SIZE
        && file_size("/input/tree/inside.tar") == TREE_ARCHIVE_SIZE
        && file_size("/output/directory.tar") == 1536
        && file_size("/output/empty.tar") == 1536
        && file_size("/output/ascii.tar") == 2048
        && file_size("/output/unicode.tar") == 2048
        && !sizeof(query_call_outs(writer))) {
        verify_tar(saw_partial && growths > 1, "large file spans elapsed callbacks");
        verify_tar(saw_overlap, "other jobs complete while large file is pending");
        verify_tar(!sizeof(query_call_outs(writer)), "all writer continuations retire");
        verify_tar(read_bytes("/output/directory.tar", -1024) == to_bytes(allocate(1024, 0)), "empty-directory work list drains and finishes");
        verify_tar(read_bytes("/output/empty.tar", -1024) == to_bytes(allocate(1024, 0)), "empty-file archive has both footer blocks");
        dump("TAR_GROWTHS: " + growths + "\n");
        dump("TAR_TOTAL: " + checks + "\n");
        efun::shutdown(0);
        return;
    }
    if (++polls > 800) throw("TAR_FAIL: archive completion deadline");
    call_out(wait_for_archives, 0.05);
}

private void run_tests() {
    "/contrib/tar"->tar("../input/tree/./empty.bin", "../output/../output/empty.tar");
    writer = find_object("/contrib/tar");
    verify_tar(file_size("/output/empty.tar") == 0, "relative paths normalize before deferred work");
    refused("/absent", "/output/rejected.tar", "missing or inaccessible source", "missing source refused");
    refused("", "/output/rejected.tar", "empty path", "empty source refused");
    refused("/oversize.bin", "/output/rejected.tar", "11-digit octal size field", "oversize source refused");
    refused("/" + "n" * 101, "/output/rejected.tar", "100 bytes", "overlong ASCII member refused");
    refused("/" + "é" * 51, "/output/rejected.tar", "100 bytes", "overlong UTF-8 member refused");
    refused("/" + "d" * 100, "/output/rejected.tar", "100 bytes", "directory slash counts toward member limit");
    refused("/input/tree/one.bin", "/input/tree/./one.bin", "must differ", "source archive alias refused");
    verify_tar(read_bytes("/input/tree/one.bin") == to_bytes(({65})), "self-target refusal preserves source");
    verify_tar(read_bytes("/output/rejected.tar") == to_bytes("keep", "UTF-8"), "validation failures preserve existing archive");
    refused("/input/tree/empty.bin", "/absent/output.tar", "parent directory", "missing archive parent refused");

    writer->tar("/input/emptydir", "/output/directory.tar");
    writer->tar("/input/tree/", "/input/tree/inside.tar");
    writer->tar("/input/large.bin", "/output/large.tar");
    writer->tar("/" + "n" * 100, "/output/ascii.tar");
    writer->tar("/" + "é" * 50, "/output/unicode.tar");
    verify_tar(file_size("/input/tree/inside.tar") == 0, "existing output is truncated before writing");
    verify_tar(file_size("/output/large.tar") == 0, "large output starts empty");
    verify_tar(file_size("/output/ascii.tar") == 0 && file_size("/output/unicode.tar") == 0, "exact 100-byte member names accepted");
    mixed *rows = query_call_outs(writer);
    verify_tar(sizeof(rows) == 6, "concurrent jobs have independent initial callbacks");
    foreach (mixed *row in rows)
        if (row[0] != writer || !functionp(row[1]) || row[3] != 0)
            throw("TAR_FAIL: bound one-shot callback shape");
    verify_tar(1, "writer owns bound one-shot callbacks");
    call_out(wait_for_archives, 0.05);
}

string *epilog(int load_empty) {
    call_out(run_tests, 0);
    return ({});
}
