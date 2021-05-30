package lte.my;

simple SpyHost
{
    parameters:
        @class(inet::SpyHost);
        @display("i=device/server");
	gates:
		input in[5];
		output out[5];
}
