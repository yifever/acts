// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <array>
#include <tuple>
#include <type_traits>
#include "ACTS/Utilities/Interpolation.hpp"
#include "ACTS/Utilities/detail/global_bin_helper.hpp"
#include "ACTS/Utilities/detail/grid_bins_helper.hpp"

namespace Acts {

namespace detail {

  /// @brief class for describing a regular multi-dimensional grid
  ///
  /// @tparam T    type of values stored inside the bins of the grid
  /// @tparam Axes parameter pack of axis types defining the grid
  ///
  /// Class describing a multi-dimensional, regular grid which can store objects
  /// in its multi-dimensional bins. Bins are hyper-cubes and can be accessed
  /// either by global bin index, local bin indices or position.
  ///
  /// @note @c T must be default-constructible.
  template <typename T, class... Axes>
  class Grid final
  {
    /// number of dimensions of the grid
    static constexpr size_t DIM = sizeof...(Axes);

  public:
    typedef T                 value_type;
    typedef value_type&       reference;
    typedef const value_type& const_reference;

    Grid(std::tuple<Axes...> axes) : m_axes(std::move(axes))
    {
      m_values.resize(size());
    }

    /// @brief access value stored in bin for a given point
    ///
    /// @tparam Point any type with point semantics supporting component access
    ///               through @c operator[]
    /// @param [in] point point used to look up the corresponding bin in the
    ///                   grid
    /// @return reference to value stored in bin containing the given point
    template <class Point>
    reference
    at(const Point& point)
    {
      return m_values.at(getGlobalBinIndex(point));
    }

    /// @brief access value stored in bin for a given point
    ///
    /// @tparam Point any type with point semantics supporting component access
    ///               through @c operator[]
    /// @param [in] point point used to look up the corresponding bin in the
    ///                   grid
    /// @return const-reference to value stored in bin containing the given
    ///         point
    template <class Point>
    const_reference
    at(const Point& point) const
    {
      return m_values.at(getGlobalBinIndex(point));
    }

    /// @brief access value stored in bin with given global bin number
    ///
    /// @param  [in] bin global bin number
    /// @return reference to value stored in bin containing the given
    ///         point
    reference
    at(size_t bin)
    {
      return m_values.at(bin);
    }

    /// @brief access value stored in bin with given global bin number
    ///
    /// @param  [in] bin global bin number
    /// @return const-reference to value stored in bin containing the given
    ///         point
    const_reference
    at(size_t bin) const
    {
      return m_values.at(bin);
    }

    /// @brief access value stored in bin with given local bin numbers
    ///
    /// @param  [in] localBins local bin indices along each axis
    /// @return reference to value stored in bin containing the given
    ///         point
    reference
    at(const std::array<size_t, DIM>& localBins)
    {
      return m_values.at(getGlobalBinIndex(localBins));
    }

    /// @brief access value stored in bin with given local bin numbers
    ///
    /// @param  [in] localBins local bin indices along each axis
    /// @return const-reference to value stored in bin containing the given
    ///         point
    ///
    /// @pre All local bin indices must be a valid index for the corresponding
    ///      axis (including the under-/overflow bin for this axis).
    const_reference
    at(const std::array<size_t, DIM>& localBins) const
    {
      return m_values.at(getGlobalBinIndex(localBins));
    }

    /// @brief dimensionality of grid
    ///
    /// @return number of axes spanning the grid
    static constexpr size_t
    dimension()
    {
      return DIM;
    }

    /// @brief get center position of bin with given local bin numbers
    ///
    /// @param  [in] localBins local bin indices along each axis
    /// @return center position of bin
    ///
    /// @pre All local bin indices must be a valid index for the corresponding
    ///      axis (excluding the under-/overflow bins for each axis).
    std::array<double, DIM>
    getBinCenter(const std::array<size_t, DIM>& localBins) const
    {
      return grid_bins_helper::getBinCenter(localBins, m_axes);
    }

    /// @brief determine global index for bin containing the given point
    ///
    /// @tparam Point any type with point semantics supporting component access
    ///               through @c operator[]
    ///
    /// @param  [in] point point to look up in the grid
    /// @return global index for bin containing the given point
    ///
    /// @note This could be a under-/overflow bin along one or more axes.
    template <class Point>
    size_t
    getGlobalBinIndex(const Point& point) const
    {
      return global_bin_helper::getGlobalBin(point, m_axes);
    }

    /// @brief determine global bin index from local bin indices along each axis
    ///
    /// @param  [in] localBins local bin indices along each axis
    /// @return global index for bin defined by the local bin indices
    ///
    /// @pre All local bin indices must be a valid index for the corresponding
    ///      axis (including the under-/overflow bin for this axis).
    size_t
    getGlobalBinIndex(const std::array<size_t, DIM>& localBins) const
    {
      return global_bin_helper::getGlobalBin(localBins, m_axes);
    }

    /// @brief determine local bin index for each axis from global bin index
    ///
    /// @param  [in] bin global bin index
    /// @return array with local bin indices along each axis (in same order as
    ///         given @c axes object)
    ///
    /// @note Local bin indices can contain under-/overflow bins along the
    ///       corresponding axis.
    std::array<size_t, DIM>
    getLocalBinIndices(size_t bin) const
    {
      return global_bin_helper::getLocalBinIndices(bin, m_axes);
    }

    /// @brief retrieve lower-left bin edge from set of local bin indices
    ///
    /// @param  [in] localBins local bin indices along each axis
    /// @return generalized lower-left bin edge position
    ///
    /// @pre @c localBins must only contain valid bin indices (i.e. excluding
    ///      under-/overflow bins).
    std::array<double, DIM>
    getLowerLeftBinEdge(const std::array<size_t, DIM>& localBins) const
    {
      return grid_bins_helper::getLowerLeftBinEdge(localBins, m_axes);
    }

    /// @brief retrieve upper-right bin edge from set of local bin indices
    ///
    /// @param  [in] localBins local bin indices along each axis
    /// @return generalized upper-right bin edge position
    ///
    /// @pre @c localBins must only contain valid bin indices (i.e. excluding
    ///      under-/overflow bins).
    std::array<double, DIM>
    getUpperRightBinEdge(const std::array<size_t, DIM>& localBins) const
    {
      return grid_bins_helper::getUpperRightBinEdge(localBins, m_axes);
    }

    /// @brief interpolate grid values to given position
    ///
    /// @tparam Point type specifying geometric positions
    /// @tparam U     dummy template parameter identical to @c T
    ///
    /// @param [in] point location to which to interpolate grid values. The
    ///                   position must be within the grid dimensions and not
    ///                   lie in an under-/overflow bin along any axis.
    ///
    /// @return interpolated value at given position
    ///
    /// @note This function is available only if the following conditions are
    /// fulfilled:
    /// - Given @c U and @c V of value type @c T as well as two @c double @c a
    /// and @c b, then the following must be a valid expression <tt>a * U + b *
    /// V</tt> yielding an object which is (implicitly) convertible to @c T.
    /// - @c Point must represent a d-dimensional position and support
    /// coordinate access using @c operator[] which should return a @c double
    /// (or a value which is implicitly convertible). Coordinate indices must
    /// start at 0.
    template <class Point,
              typename U = T,
              typename
              = std::enable_if_t<can_interpolate<Point,
                                                 std::array<double, DIM>,
                                                 std::array<double, DIM>,
                                                 U>::value>>
    T
    interpolate(const Point& point) const
    {
      // there are 2^DIM corner points used during the interpolation
      constexpr size_t nCorners = 1 << DIM;

      // construct vector of pairs of adjacent bin centers and values
      std::array<value_type, nCorners> neighbors;

      // get local indices for current bin
      // value of bin is interpreted as being the field value at its lower left
      // corner
      const auto& llIndices = getLocalBinIndices(getGlobalBinIndex(point));

      // get local indices for "upper right" bin (i.e. all local indices
      // incremented by one but avoid overflows)
      auto urIndices
          = grid_bins_helper::getUpperRightBinIndices(llIndices, m_axes);

      // loop through all corner points
      for (size_t i = 0; i < nCorners; ++i) {
        auto indices = llIndices;
        for (size_t dimension = 0; dimension < DIM; ++dimension) {
          if (i & (1 << dimension)) indices.at(dimension) += 1;
        }
        neighbors.at(i) = at(indices);
      }

      return Acts::interpolate(point,
                               getLowerLeftBinEdge(llIndices),
                               getLowerLeftBinEdge(urIndices),
                               neighbors);
    }

    /// @brief total number of bins
    ///
    /// @return total number of bins in the grid
    ///
    /// @note This number contains under-and overflow bins along all axes.
    size_t
    size() const
    {
      return grid_bins_helper::getNBins(m_axes);
    }

  private:
    /// set of axis defining the multi-dimensional grid
    std::tuple<Axes...> m_axes;
    /// linear value store for each bin
    std::vector<T> m_values;
  };
}  // namespace detail

}  // namespace Acts
