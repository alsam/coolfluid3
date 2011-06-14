// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_RDM_SchemeBase_hpp
#define CF_RDM_SchemeBase_hpp

#include <functional>

#include <boost/assign.hpp>

#include <Eigen/Dense>

#include "Common/Core.hpp"
#include "Common/OptionT.hpp"
#include "Common/BasicExceptions.hpp"

#include "Math/MatrixTypes.hpp"

#include "Mesh/ElementData.hpp"
#include "Mesh/CField.hpp"
#include "Mesh/CFieldView.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/ElementType.hpp"

#include "Solver/Actions/CLoopOperation.hpp"

#include "RDM/Core/LibCore.hpp"
#include "RDM/Core/CellLoop.hpp"

/////////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace RDM {

///////////////////////////////////////////////////////////////////////////////////////

template < typename SF, typename QD, typename PHYS >
class RDM_CORE_API SchemeBase : public Solver::Actions::CLoopOperation {

public: // typedefs

  /// pointers
  typedef boost::shared_ptr< SchemeBase > Ptr;
  typedef boost::shared_ptr< SchemeBase const> ConstPtr;

public: // functions

  /// Contructor
  /// @param name of the component
  SchemeBase ( const std::string& name );

  /// Virtual destructor
  virtual ~SchemeBase() {};

  /// Get the class name
  static std::string type_name () { return "SchemeBase<" + SF::type_name() + ">"; }
	
  /// interpolates the shape functions and gradient values
  /// @post zeros the local residual matrix
  void interpolate ( const Mesh::CTable<Uint>::ConstRow& nodes_idx );

  void sol_gradients_at_qdpoint(const Uint q);

protected: // helper functions

  void change_elements()
  { 
    /// @todo improve this (ugly)

    connectivity_table = elements().as_ptr<Mesh::CElements>()->node_connectivity().as_ptr< Mesh::CTable<Uint> >();
    coordinates = elements().nodes().coordinates().as_ptr< Mesh::CTable<Real> >();

    cf_assert( is_not_null(connectivity_table) );

    /// @todo modify these to option components configured from

    Mesh::CField::Ptr csolution = Common::find_component_ptr_recursively_with_tag<Mesh::CField>( Common::Core::instance().root(), "solution" );
    cf_assert( is_not_null( csolution ) );
    solution = csolution->data_ptr();

    Mesh::CField::Ptr cresidual = Common::find_component_ptr_recursively_with_tag<Mesh::CField>( Common::Core::instance().root(), "residual" );
    cf_assert( is_not_null( cresidual ) );
    residual = cresidual->data_ptr();

    Mesh::CField::Ptr cwave_speed = Common::find_component_ptr_recursively_with_tag<Mesh::CField>( Common::Core::instance().root(), "wave_speed" );
    cf_assert( is_not_null( cwave_speed ) );
    wave_speed = cwave_speed->data_ptr();
  }

protected: // typedefs

  typedef typename SF::NodeMatrixT                             NodeMT;

  typedef Eigen::Matrix<Real, QD::nb_points, 1u>               WeightVT;

  typedef Eigen::Matrix<Real, QD::nb_points, PHYS::neqs>       ResidualMT;

  typedef Eigen::Matrix<Real, PHYS::neqs, PHYS::neqs >         EigenValueMT;

  typedef Eigen::Matrix<Real, PHYS::neqs, PHYS::neqs>          PhysicsMT;
  typedef Eigen::Matrix<Real, PHYS::neqs, 1u>                  PhysicsVT;

  typedef Eigen::Matrix<Real, SF::nb_nodes,   PHYS::neqs>      SolutionMT;
  typedef Eigen::Matrix<Real, 1u, PHYS::neqs >                 SolutionVT;

  typedef Eigen::Matrix<Real, QD::nb_points, SF::nb_nodes>     SFMatrixT;
  typedef Eigen::Matrix<Real, 1u, SF::nb_nodes >               SFVectorT;

  typedef Eigen::Matrix<Real, PHYS::ndim, 1u>                  DimVT;

  typedef Eigen::Matrix<Real, PHYS::ndim, PHYS::ndim>          JMT;

  typedef Eigen::Matrix<Real, QD::nb_points, PHYS::ndim>       QCoordMT;
  typedef Eigen::Matrix<Real, QD::nb_points, PHYS::neqs>       QSolutionMT;

  typedef Eigen::Matrix<Real, PHYS::neqs, PHYS::ndim>          QSolutionVT;

protected: // data

  /// pointer to connectivity table, may reset when iterating over element types
  Mesh::CTable<Uint>::Ptr connectivity_table;
  /// pointer to nodes coordinates, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr coordinates;
  /// pointer to solution table, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr solution;
  /// pointer to solution table, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr residual;
  /// pointer to solution table, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr wave_speed;

  /// helper object to compute the quadrature information
  const QD& m_quadrature;

  /// coordinates of quadrature points in physical space
  QCoordMT X_q;
  /// stores dX/dksi and dx/deta at each quadrature point, one matrix per dimension
  QCoordMT dX[PHYS::ndim];

  /// solution at quadrature points in physical space
  QSolutionMT U_q;
  /// derivatives of solution to X at each quadrature point, one matrix per dimension
  QSolutionMT dUdX[PHYS::ndim];

  /// derivatives of solution to X at ONE quadrature point, each column stores derivatives
  /// with respect to given coordinate
  QSolutionVT dUdXq;

  /// contribution to nodal residuals
  SolutionMT Phi_n;
  /// node values
  NodeMT     X_n;
  /// Values of the solution located in the dof of the element
  SolutionMT U_n;
  /// Values of the operator L(u) computed in quadrature points.
  PhysicsVT  LU;
  /// flux jacobians
  PhysicsMT  dFdU[PHYS::ndim];
  /// interporlation matrix - values of shapefunction at each quadrature point
  SFMatrixT  Ni;
  /// derivative matrix - values of shapefunction derivative in Ksi at each quadrature point
  SFMatrixT  dNdKSI[PHYS::ndim];
  /// derivatives of shape functions on physical space at all quadrature points,
  /// one matrix per dimenison
  SFMatrixT  dNdX[PHYS::ndim];
  /// jacobian of transformation at each quadrature point
  WeightVT jacob;
  /// Integration factor (jacobian multiplied by quadrature weight)
  WeightVT wj;
  /// temporary local gradient of 1 shape function
  DimVT dN;
  /// physical properties
  typename PHYS::Properties phys_props;

private:

  /// temporary local gradient of 1 shape function in reference and physical space
  DimVT dNphys;
  DimVT dNref;
  /// Jacobi matrix at each quadrature point
  JMT JM;
  /// Inverse of the Jacobi matrix at each quadrature point
  JMT JMinv;

};

///////////////////////////////////////////////////////////////////////////////////////

template<typename SF, typename QD, typename PHYS>
SchemeBase<SF,QD,PHYS>::SchemeBase ( const std::string& name ) :
  CLoopOperation(name),
  m_quadrature( QD::instance() )
{ 
  regist_typeinfo(this);

  m_properties["Elements"].as_option().attach_trigger ( boost::bind ( &SchemeBase<SF,QD,PHYS>::change_elements, this ) );

  for(Uint d = 0; d < PHYS::ndim; ++d)
    dFdU[d].setZero();

  // Gradient of the shape functions in reference space
  typename SF::MappedGradientT GradSF;
  // Values of shape functions in reference space
  typename SF::ShapeFunctionsT ValueSF;

  // initialize the interpolation matrix

  for(Uint q = 0; q < QD::nb_points; ++q)
    for(Uint n = 0; n < SF::nb_nodes; ++n)
    {
       SF::shape_function_gradient( m_quadrature.coords.col(q), GradSF  );
       SF::shape_function_value ( m_quadrature.coords.col(q), ValueSF );

       Ni(q,n) = ValueSF[n];

       for(Uint d = 0; d < PHYS::ndim; ++d)
         dNdKSI[d](q,n) = GradSF(d,n);
    }

  // debug

  //  std::cout << "QD points [" << QD::nb_points << "]"  << std::endl;
  //  std::cout << "Ki_qn   is " << Ki_n[0].rows()<< "x" << Ki_n[0].cols() << std::endl;
  //  std::cout << "LU      is " << LU.rows() << "x" << LU.cols()  << std::endl;
  //  std::cout << "Phi_n   is " << Phi_n.rows()   << "x" << Phi_n.cols()    << std::endl;
  //  std::cout << "U_n     is " << U_n.rows()     << "x" << U_n.cols()      << std::endl;
  //  std::cout << "DvPlus  is " << DvPlus[0].rows()  << "x" << DvPlus[0].cols()   << std::endl;

}

/////////////////////////////////////////////////////////////////////////////////////

template<typename SF,typename QD, typename PHYS>
void SchemeBase<SF, QD,PHYS>::interpolate( const Mesh::CTable<Uint>::ConstRow& nodes_idx )
{
  /// @todo must be tested for 3D

  // copy the coordinates from the large array to a small

  Mesh::fill(X_n, *coordinates, nodes_idx );

  // copy the solution from the large array to a small

  for(Uint n = 0; n < SF::nb_nodes; ++n)
    for (Uint v=0; v < PHYS::neqs; ++v)
      U_n(n,v) = (*solution)[ nodes_idx[n] ][v];

  // coordinates of quadrature points in physical space

  X_q  = Ni * X_n;

  // solution at all quadrature points in physical space

  U_q = Ni * U_n;

  // Jacobian of transformation phys -> ref:
  //    |   dx/dksi    dx/deta    |
  //    |   dy/dksi    dy/deta    |

  // dX[XX].col(KSI) has the values of dx/dksi at all quadrature points
  // dX[XX].col(ETA) has the values of dx/deta at all quadrature points

  for(Uint dimx = 0; dimx < PHYS::ndim; ++dimx)
  {
    for(Uint dimksi = 0; dimksi < PHYS::ndim; ++dimksi)
    {
      dX[dimx].col(dimksi) = dNdKSI[dimksi] * X_n.col(dimx);
    }
  }

  // Fill Jacobi matrix (matrix of transformation phys. space -> ref. space) at qd. point q
  for(Uint q = 0; q < QD::nb_points; ++q)
  {
    for(Uint dimx = 0; dimx < PHYS::ndim; ++dimx)
    {
      for(Uint dimksi = 0; dimksi < PHYS::ndim; ++dimksi)
      {
       JM(dimksi,dimx) = dX[dimx](q,dimksi);
      }
    }

   // Once the jacobi matrix at one quadrature point is assembled, let's re-use it
   // to compute the gradients of of all shape functions in phys. space
   jacob[q] = JM.determinant();
   JMinv = JM.inverse();

   for(Uint n = 0; n < SF::nb_nodes; ++n)
   {
     for(Uint dimksi = 0; dimksi < PHYS::ndim; ++ dimksi)
     {
       dNref[dimksi] = dNdKSI[dimksi](q,n);
     }

     dNphys = JMinv * dNref;

     for(Uint dimx = 0; dimx < PHYS::ndim; ++ dimx)
     {
       dNdX[dimx](q,n) = dNphys[dimx];
     }

   }

  } // loop over quadrature points

  // compute transformed integration weights (sum is element area)

  for(Uint q = 0; q < QD::nb_points; ++q)
    wj[q] = jacob[q] * m_quadrature.weights[q];

  // solution derivatives in physical space at quadrature point

  dUdX[XX] = dNdX[XX] * U_n;
  dUdX[YY] = dNdX[YY] * U_n;

  // zero element residuals

  Phi_n.setZero();
}

/////////////////////////////////////////////////////////////////////////////////////

template<typename SF,typename QD, typename PHYS>
void SchemeBase<SF, QD,PHYS>::sol_gradients_at_qdpoint(const Uint q)
{
  for(Uint dim = 0; dim < PHYS::ndim; ++dim)
  {
    dUdXq.col(dim) = dUdX[dim].row(q).transpose();
  }
}

////////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF

/////////////////////////////////////////////////////////////////////////////////////

#endif // CF_RDM_SchemeBase_hpp
