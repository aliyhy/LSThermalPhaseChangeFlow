/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

#include "thermalPhaseChangeTwoPhaseMixture.H"
#include "thermalIncompressibleTwoPhaseMixture.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::thermalPhaseChangeTwoPhaseMixture>
Foam::thermalPhaseChangeTwoPhaseMixture::New
(
    const volVectorField& U,
    const surfaceScalarField& phi
)
{
    const IOdictionary dict
    (
        IOobject
        (
            "transportProperties",
            U.time().constant(),
            U.db(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE,
            false // Do not register
        )
    );

    const word modelType(dict.get<word>("thermalPhaseChangeTwoPhaseMixture"));

    Info<< "Selecting phaseChange model " << modelType << endl;

    auto cstrIter = componentsConstructorTablePtr_->cfind(modelType);

    if (!cstrIter.found())
    {
        FatalIOErrorInLookup
        (
            dict,
            "thermalPhaseChangeTwoPhaseMixture",
            modelType,
            *componentsConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    return autoPtr<thermalPhaseChangeTwoPhaseMixture>(cstrIter()(U, phi));
}


// ************************************************************************* //
