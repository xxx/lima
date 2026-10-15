/* Do not remove the headers from this file! see /USAGE for more info. */

//: COMMAND
//$$ see: netstat
// USAGE: ``socketinfo``
//
// Displays native socket handles, states, kinds and numeric address/port pairs.
//
// .. TAGS: RST

inherit CMD;

private string endpoint_text(mixed endpoint)
{
   return endpoint ? sprintf("%s %d", endpoint[0], endpoint[1]) : "-";
}

private void main()
{
   write("Handle  State       Kind      Local Address          Remote Address\n"
         "------  ----------  --------  ---------------------  ---------------------\n");
   foreach (mapping item in query_sockets(-1))
      write(sprintf("%6d  %-10s  %-8s  %-21s  %s\n", item["handle"], item["state"], item["kind"],
                    endpoint_text(item["local"]), endpoint_text(item["remote"])));
}

protected varargs void dispatch_main(mixed arg, mixed flags, mixed input, mixed implode_info)
{
   main();
}
