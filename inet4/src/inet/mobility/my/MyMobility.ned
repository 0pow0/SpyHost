package inet.mobility.my;

import inet.mobility.contract.IMobility;

//
// Abstract base module for mobility models.
//
// Format directives:
//  - %p position
//  - %v velocity
//  - %s speed
//  - %a acceleration
//  - %P angular position
//  - %V angular velocity
//  - %S angular speed
//  - %A angular acceleration
//
// @author Andras Varga
//
simple MyMobility like IMobility
{
    parameters:
        @signal[mobilityStateChanged](type=inet::MobilityBase);     // it's work in inet, but not work in extended module in other namespace
        @display("i=block/cogwheel");
        string subjectModule = default("^"); // module path which determines the subject module, the motion of which this mobility model describes, the default value is the parent module
        string coordinateSystemModule = default(""); // module path of the geographic coordinate system module
        string displayStringTextFormat = default("p: %p\nv: %v"); // format string for the mobility module's display string text
        bool updateDisplayString = default(true); // enables continuous update of the subject module's position via modifying its display string
        double constraintAreaMinX @unit(m) = default(-inf m); // min x position of the constraint area, unconstrained by default (negative infinity)
        double constraintAreaMinY @unit(m) = default(-inf m); // min y position of the constraint area, unconstrained by default (negative infinity)
        double constraintAreaMinZ @unit(m) = default(-inf m); // min z position of the constraint area, unconstrained by default (negative infinity)
        double constraintAreaMaxX @unit(m) = default(inf m); // max x position of the constraint area, unconstrained by default (positive infinity)
        double constraintAreaMaxY @unit(m) = default(inf m); // max y position of the constraint area, unconstrained by default (positive infinity)
        double constraintAreaMaxZ @unit(m) = default(inf m); // max z position of the constraint area, unconstrained by default (positive infinity)
        double updateInterval @unit(s) = default(0.1s); // the simulation time interval used to regularly signal mobility state changes and update the display
        bool faceForward = default(true);

		double speed @unit(mps) = default(0mps);
		double initialMovementHeading @unit(deg) = default(uniform(0deg, 360deg));
        double initialMovementElevation @unit(deg) = default(0deg);
    gates:
    	input in;
    	output out;	
}
