package lte.my;

simple SpyHost
{
    parameters:
        @class(inet::SpyHost);
        @display("i=device/server");
        int numUe = default(1);
	gates:
		input in[numUe];
		output out[numUe];
}
