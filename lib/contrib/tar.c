/* Do not remove the headers from this file! see /USAGE for more info. */

/*
**	Posix tar object in LPC. Zellski, zell@alyx.com, 1996.
**
**	Written for Igor Mud, a DGD-based 2.4.5. Should be fairly
**	portable -- does need mutable, non-zero-sensitive strings.
**
**	Anybody is free to do anything with this code as far as I
**	am concerned. Of course, I also consider myself free to call
**	people bloody bastards when they are.
**
**      MudOS port by Beek
*/

#define TAR_FILES_PER_CALLBACK 10
#define TAR_BYTES_PER_CALLBACK (1024 * 1024)
#define TAR_MAX_FILE_SIZE 8589934591

#define LF_DIR 5
#define LF_NORMAL 0

#define UID 1701
#define GID 2261
#define UNAME "zell"
#define GNAME "alyx"

private class tar_job {
   string *files;
   int index;
   string archive;
   string current;
   int size;
   int offset;
   int written;
}

private string tar_path(string path)
{
   if (path == "")
      error("tar: an empty path is not allowed");
   path = canonical_path(absolute_path(path, this_object()));
   if (sizeof(path) > 1 && path[<1] == '/')
      path = path[0..<2];
   return path;
}

private string member_name(string path, int type)
{
   string name = path == "/" ? "." : path[1..];
   if (type == LF_DIR)
      name += "/";
   if (sizeof(to_bytes(name, "UTF-8")) > 100)
      error("tar: member name exceeds 100 bytes: " + name);
   return name;
}

private bytes octal(int num, int width)
{
   int *result = allocate(width, ' ');
   int i = width - 1;
   do {
      if (i == 0)
         error("tar: number does not fit its header field");
      result[--i] = '0' + (num & 7);
      num >>= 3;
   } while (num);
   return to_bytes(result);
}

private void header_insert(int *header, bytes value, int offset, int size)
{
   for (int i = 0; i < size; i++)
      header[offset + i] = i < sizeof(value) ? value[i] : 0;
}

private bytes make_header(string path, int size, int type)
{
   int *header = allocate(512, 0);
   header_insert(header, to_bytes(member_name(path, type), "UTF-8"), 0, 100);
   header_insert(header, to_bytes(" 40755 ", "UTF-8"), 100, 8);
   header_insert(header, octal(UID, 7), 108, 8);
   header_insert(header, octal(GID, 7), 116, 8);
   header_insert(header, octal(size, 12), 124, 12);
   header_insert(header, octal(time(), 12), 136, 12);
   header_insert(header, to_bytes("        ", "UTF-8"), 148, 8);
   header_insert(header, to_bytes("" + type, "UTF-8"), 156, 1);
   header_insert(header, to_bytes("ustar  ", "UTF-8"), 257, 8);
   header_insert(header, to_bytes(UNAME, "UTF-8"), 265, 32);
   header_insert(header, to_bytes(GNAME, "UTF-8"), 297, 32);
   int sum;
   foreach (int value in header)
      sum += value;
   header_insert(header, octal(sum, 7)[0..<2] + to_bytes(({0, ' '})), 148, 8);
   return to_bytes(header);
}

private void append_bytes(class tar_job job, bytes data)
{
   if (!write_bytes(job.archive, job.written, data))
      error("tar: failed to write " + job.archive);
   job.written += sizeof(data);
}

private void add_next_files(class tar_job job)
{
   int count;
   int budget = TAR_BYTES_PER_CALLBACK;
   while (count < TAR_FILES_PER_CALLBACK && budget > 0)
   {
      if (job.current != "")
      {
         int length = job.size - job.offset;
         if (length > budget)
            length = budget;
         mixed data = read_bytes(job.current, job.offset, length);
         if (!bytesp(data) || sizeof(data) != length)
            error("tar: short read from " + job.current);
         append_bytes(job, data);
         job.offset += length;
         budget -= length;
         if (job.offset == job.size)
         {
            int padding = (512 - job.size % 512) % 512;
            if (padding)
               append_bytes(job, to_bytes(allocate(padding, 0)));
            write(job.current + "\tdone.\n");
            job.current = "";
         }
         continue;
      }

      if (job.index == sizeof(job.files))
      {
         append_bytes(job, to_bytes(allocate(1024, 0)));
         write("DONE.\n");
         return;
      }

      string path = job.files[job.index++];
      count++;
      if (path == job.archive)
         continue;
      int size = file_size(path);
      if (size == -2)
      {
         append_bytes(job, make_header(path, 0, LF_DIR));
         string prefix = path == "/" ? "/" : path + "/";
         string *names = get_dir(prefix);
         for (int i = 0; i < sizeof(names); i++)
            names[i] = prefix + names[i];
         job.files = names + job.files[job.index..];
         job.index = 0;
      }
      else
      {
         if (size < 0)
            error("tar: missing or inaccessible file: " + path);
         if (size > TAR_MAX_FILE_SIZE)
            error("tar: file exceeds the 11-digit octal size field: " + path);
         append_bytes(job, make_header(path, size, LF_NORMAL));
         if (size)
         {
            job.current = path;
            job.size = size;
            job.offset = 0;
         }
         else
            write(path + "\tdone.\n");
      }
   }
   call_out(&add_next_files(job), 1);
}

void tar(string dir, string archive)
{
   dir = tar_path(dir);
   archive = tar_path(archive);
   if (dir == archive)
      error("tar: the source and archive must differ");
   int size = file_size(dir);
   if (size == -1)
      error("tar: missing or inaccessible source: " + dir);
   if (size > TAR_MAX_FILE_SIZE)
      error("tar: file exceeds the 11-digit octal size field: " + dir);
   member_name(dir, size == -2 ? LF_DIR : LF_NORMAL);
   if (!write_file(archive, "", 1))
      error("tar: failed to create " + archive);
   class tar_job job = new(class tar_job,
      files: ({dir}), index: 0, archive: archive, current: "",
      size: 0, offset: 0, written: 0);
   call_out(&add_next_files(job), 0);
}
