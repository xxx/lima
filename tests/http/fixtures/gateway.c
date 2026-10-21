private int calls;
private mixed argument;
private mapping headers;
void reset_calls() { calls = 0; argument = nil; headers = nil; }
int call_count() { return calls; }
mixed last_argument() { return argument; }
mapping last_headers() { return headers; }
varargs string main(mixed data, mapping request_headers)
{
   calls++; argument = data; headers = request_headers;
   return "CGI é\n";
}
