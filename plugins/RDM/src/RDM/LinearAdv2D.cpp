// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.


#include "RDM/LinearAdv2D.hpp"

/////////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace RDM {

///////////////////////////////////////////////////////////////////////////////////////

LinearAdv2D::LinearAdv2D()
{
}

/////////////////////////////////////////////////////////////////////////////////////

LinearAdv2D::~LinearAdv2D()
{
}

/////////////////////////////////////////////////////////////////////////////////////

Real LinearAdv2D::flux(const RealVector2 & point, const RealVector2 & grad)
{
   return point[YY]*grad[XX] - point[XX]*grad[YY];
}


} // RDM
} // CF

/////////////////////////////////////////////////////////////////////////////////////
