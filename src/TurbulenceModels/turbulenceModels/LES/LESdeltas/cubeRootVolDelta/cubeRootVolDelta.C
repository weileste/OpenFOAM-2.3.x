/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
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

#include "cubeRootVolDelta.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(cubeRootVolDelta, 0);
    addToRunTimeSelectionTable(LESdelta, cubeRootVolDelta, dictionary);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::cubeRootVolDelta::calcDelta()
{
    label nD = mesh().nGeometricD();

    if (nD == 3)
    {
        delta_.internalField() = deltaCoeff_*pow(mesh().V(), 1.0/3.0);
    }
    else if (nD == 2)
    {
        WarningIn("cubeRootVolDelta::calcDelta()")
            << "Case is 2D, LES is not strictly applicable\n"
            << endl;

        const Vector<label>& directions = mesh().geometricD();

        scalar thickness = 0.0;
        for (direction dir=0; dir<directions.nComponents; dir++)
        {
            if (directions[dir] == -1)
            {
                thickness = mesh().bounds().span()[dir];
                break;
            }
        }

        delta_.internalField() = deltaCoeff_*sqrt(mesh().V()/thickness);
    }
    else
    {
        FatalErrorIn("cubeRootVolDelta::calcDelta()")
            << "Case is not 3D or 2D, LES is not applicable"
            << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::cubeRootVolDelta::cubeRootVolDelta
(
    const word& name,
    const fvMesh& mesh,
    const dictionary& dict
)
:
    LESdelta(name, mesh),
    deltaCoeff_
    (
        dict.subDict(type() + "Coeffs").lookupOrDefault<scalar>("deltaCoeff", 1)
    )
{
    calcDelta();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::cubeRootVolDelta::read(const dictionary& dict)
{
    dict.subDict(type() + "Coeffs").readIfPresent<scalar>
    (
        "deltaCoeff",
        deltaCoeff_
    );

    calcDelta();
}


void Foam::cubeRootVolDelta::correct()
{
    if (mesh_.changing())
    {
        calcDelta();
    }
}


// ************************************************************************* //
