/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011-2014, Willow Garage, Inc.
 *  Copyright (c) 2014-2016, Open Source Robotics Foundation
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Open Source Robotics Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Jia Pan */


#ifndef FCL_BVH_UTILITY_H
#define FCL_BVH_UTILITY_H

#include "fcl/math/variance3.h"
#include "fcl/BVH/BVH_model.h"

namespace fcl
{
/// @brief Expand the BVH bounding boxes according to the variance matrix
/// corresponding to the data stored within each BV node
template <typename BV>
void BVHExpand(
    BVHModel<BV>& model,
    const Variance3<typename BV::Scalar>* ucs,
    typename BV::Scalar r);

/// @brief Expand the BVH bounding boxes according to the corresponding variance
/// information, for OBB
template <typename Scalar>
void BVHExpand(
    BVHModel<OBB<Scalar>>& model, const Variance3<Scalar>* ucs, Scalar r = 1.0);

/// @brief Expand the BVH bounding boxes according to the corresponding variance
/// information, for RSS
template <typename Scalar>
void BVHExpand(
    BVHModel<RSS<Scalar>>& model, const Variance3<Scalar>* ucs, Scalar r = 1.0);

//============================================================================//
//                                                                            //
//                              Implementations                               //
//                                                                            //
//============================================================================//

//==============================================================================
template <typename BV>
void BVHExpand(
    BVHModel<BV>& model,
    const Variance3<typename BV::Scalar>* ucs,
    typename BV::Scalar r)
{
  using Scalar = typename BV::Scalar;

  for(int i = 0; i < model.num_bvs; ++i)
  {
    BVNode<BV>& bvnode = model.getBV(i);

    BV bv;
    for(int j = 0; j < bvnode.num_primitives; ++j)
    {
      int v_id = bvnode.first_primitive + j;
      const Variance3<Scalar>& uc = ucs[v_id];

      Vector3<Scalar>& v = model.vertices[bvnode.first_primitive + j];

      for(int k = 0; k < 3; ++k)
      {
        bv += (v + uc.axis.col(k) * (r * uc.sigma[k]));
        bv += (v - uc.axis.col(k) * (r * uc.sigma[k]));
      }
    }

    bvnode.bv = bv;
  }
}

//==============================================================================
template <typename Scalar>
void BVHExpand(
    BVHModel<OBB<Scalar>>& model,
    const Variance3<Scalar>* ucs,
    Scalar r)
{
  for(int i = 0; i < model.getNumBVs(); ++i)
  {
    BVNode<OBB<Scalar>>& bvnode = model.getBV(i);

    Vector3<Scalar>* vs = new Vector3<Scalar>[bvnode.num_primitives * 6];

    for(int j = 0; j < bvnode.num_primitives; ++j)
    {
      int v_id = bvnode.first_primitive + j;
      const Variance3<Scalar>& uc = ucs[v_id];

      Vector3<Scalar>&v = model.vertices[bvnode.first_primitive + j];

      for(int k = 0; k < 3; ++k)
      {
        vs[6 * j + 2 * k] = v + uc.axis.col(k) * (r * uc.sigma[k]);
        vs[6 * j + 2 * k + 1] = v - uc.axis.col(k) * (r * uc.sigma[k]);
      }
    }

    OBB<Scalar> bv;
    fit(vs, bvnode.num_primitives * 6, bv);

    delete [] vs;

    bvnode.bv = bv;
  }
}

//==============================================================================
template <typename Scalar>
void BVHExpand(
    BVHModel<RSS<Scalar>>& model,
    const Variance3<Scalar>* ucs,
    Scalar r)
{
  for(int i = 0; i < model.getNumBVs(); ++i)
  {
    BVNode<RSS<Scalar>>& bvnode = model.getBV(i);

    Vector3<Scalar>* vs = new Vector3<Scalar>[bvnode.num_primitives * 6];

    for(int j = 0; j < bvnode.num_primitives; ++j)
    {
      int v_id = bvnode.first_primitive + j;
      const Variance3<Scalar>& uc = ucs[v_id];

      Vector3<Scalar>&v = model.vertices[bvnode.first_primitive + j];

      for(int k = 0; k < 3; ++k)
      {
        vs[6 * j + 2 * k] = v + uc.axis.col(k) * (r * uc.sigma[k]);
        vs[6 * j + 2 * k + 1] = v - uc.axis.col(k) * (r * uc.sigma[k]);
      }
    }

    RSS<Scalar> bv;
    fit(vs, bvnode.num_primitives * 6, bv);

    delete [] vs;

    bvnode.bv = bv;
  }
}

} // namespace fcl

#endif
