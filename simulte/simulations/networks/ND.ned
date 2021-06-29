//
//                           SimuLTE
// 
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself, 
// and cannot be removed from it.
// 
package lte.simulations.networks;

import inet.networklayer.ipv4.RoutingTableRecorder;
import inet.node.ethernet.Eth10G;
import inet.node.ethernet.Eth10M;
import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import lte.corenetwork.binder.LteBinder;
import lte.corenetwork.nodes.MyUe;
import lte.corenetwork.nodes.eNodeB;
import lte.world.radio.LteChannelControl;
import lte.epc.PgwStandardSimplified;
import lte.common.LteNetworkConfigurator;
import lte.my.SpyHost;
import inet.common.geometry.common.SimpleGeographicCoordinateSystem;

network ND 
{
    parameters:
        int numeNB = default(2);
        int numUe = default(1);
        @display("bgb=55000,43845;bgi=background/ND,s;bgu=km");
        double playgroundLatitude; // geographic position of the playground's north-west corner
        double playgroundLongitude; // geographic position of the playground's north-west corner
        double playgroundWidth @unit("m") = default(300m);  // the E-W size of playground
        double playgroundHeight @unit("m") = default(300m); // the N-S size of playground
    submodules:
        channelControl: LteChannelControl {
            @display("p=50,25;is=s");
        }
        routingRecorder: RoutingTableRecorder {
            @display("p=50,75;is=s");
        }
        configurator: LteNetworkConfigurator {
            @display("p=50,125");
        }
        binder: LteBinder {
            @display("p=50,175;is=s");
        }
        server: StandardHost {
            @display("p=212,118;is=n;i=device/server");
        }
        router: Router {
            @display("p=321,136;i=device/smallrouter");
        }
        pgw: PgwStandardSimplified {
            nodeType = "PGW";
            @display("p=519,175;is=l");
        }
        eNB[numeNB]: eNodeB {
            @display("p=123,123;is=vl");
        }
        ue[numUe]: MyUe {
            @display("p=783,278");
        }
        spy: SpyHost {
            @display("p=500,500");
        }
        coordinateSystem: SimpleGeographicCoordinateSystem {
        }
        osgEarthScene: OsgEarthScene {
            @display("p=74,31");
        }
    connections:
        server.pppg++ <--> Eth10G <--> router.pppg++;
        router.pppg++ <--> Eth10G <--> pgw.filterGate;
        //pgw.pppg++ <--> Eth10G <--> eNB.ppp;
        
        for i = 0..numeNB-1 {
            pgw.pppg++ <--> Eth10G <--> eNB[i].ppp;
        }

        for i = 0..numUe-1 {
            ue[i].myMobilityIn <-- Eth10G <-- spy.out[i];
            spy.in[i] <-- Eth10G <-- ue[i].myMobilityOut;
        }

}
