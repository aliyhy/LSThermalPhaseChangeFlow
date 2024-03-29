/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    interPhaseChangeFoam

Group
    grpMultiphaseSolvers

Description
    Solver for two incompressible, isothermal immiscible fluids with
    phase-change (e.g. cavitation).
    Uses VOF (volume of fluid), Level-Set and isoAdvector interface capturing
    methods.

    The momentum and other fluid properties are of the "mixture" and a
    single momentum equation is solved.

    The set of phase-change models provided are designed to simulate cavitation
    but other mechanisms of phase-change are supported within this solver
    framework.

    Turbulence modelling is generic, i.e. laminar, RAS or LES may be selected.

\*---------------------------------------------------------------------------*/
#include "isoAdvection.H"
#include "fvCFD.H"
#include "CMULES.H"
#include "subCycle.H"
#include "interfaceProperties.H"
#include "interfacePropertiesLSH.H"
#include "thermalPhaseChangeTwoPhaseMixture.H"
#include "turbulentTransportModel.H"
#include "pimpleControl.H"
#include "fvOptions.H"
#include "surfaceForces.H"
#include "advectionSchemes.H"



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Solver for two incompressible, isothermal immiscible fluids with"
        " phase-change.\n"
        "Uses VOF (volume of fluid), Level-Set and isoAdvector interface"
        " capturing methods."
    );

    #include "postProcess.H"
    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createControl.H"
    #include "createFields.H"
    #include "createTimeControls.H"
    #include "CourantNo.H"
    #include "setInitialDeltaT.H"
    
               
    turbulence->validate();

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        #include "readTimeControls.H"
        #include "CourantNo.H"
        #include "setDeltaT.H"

        ++runTime;
        if (interfaceMethod == "isoAdvectorPLIC")
        {
            if(overwrite)
                {
                    runTime.setTime(runTime.value() - runTime.deltaTValue(), 1);
                    runTime.writeAndEnd();
                }
        }

        Info<< "Time = " << runTime.timeName() << nl << endl;
        
        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
        if (interfaceMethod == "isoAdvectorPLIC")
        {
            if(overwrite)
                {
                    continue;

                }
        }
            #include "alphaControls.H"

            surfaceScalarField rhoPhi
            (
                IOobject
                (
                    "rhoPhi",
                    runTime.timeName(),
                    mesh
                ),
                mesh,
                dimensionedScalar(dimMass/dimTime, Zero)
            );

            mixture->correct();

            #include "alphaEqnSubCycle.H"
            mixture->correct();

            interface.correct();
            
            if (interfaceMethod == "isoAdvectorPLIC")
            {
                surfForces.correct();

            }
            

            #include "UEqn.H"
            #include "TEqn.H"


            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "pEqn.H"
            }
            
            


            if (pimple.turbCorr())
            {
                turbulence->correct();
            }
        }
        rho = alpha1*rho1 + alpha2*rho2;

        runTime.write();

        runTime.printExecutionTime(Info);
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
