// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#ifndef CHEMFILES_CONNECTIVITY_HPP
#define CHEMFILES_CONNECTIVITY_HPP

#include <array>
#include <cassert>

#include "chemfiles/sorted_set.hpp"
#include "chemfiles/exports.hpp"
#include "chemfiles/Error.hpp"

namespace chemfiles {

/// The bond struct ensure a canonical representation of a bond between atoms
/// i and j, with i<j
struct CHFL_EXPORT Bond {
    Bond(size_t first, size_t second) {
        if (first == second) {
            throw Error("Can not have a bond between an atom and itself");
        }

        data_[0] = std::min(first, second);
        data_[1] = std::max(first, second);
    }

    /// Get the index of the `i`th atom (`i == 0` or `i == 1`) in the bond.
    ///
    /// @throws OutOfBounds if `i` is not 0 or 1
    const size_t& operator[](size_t i) const {
        if (i >= 2) {
            throw OutOfBounds("Can not access bond atom " + std::to_string(i));
        }
        return data_[i];
    }

    Bond(Bond&&) = default;
    Bond& operator=(Bond&&) = default;
    Bond(const Bond&) = default;
    Bond& operator=(const Bond&) = default;

private:
    std::array<size_t, 2> data_;
};

inline bool operator==(const Bond& lhs, const Bond& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1];
}

inline bool operator!=(const Bond& lhs, const Bond& rhs) {
    return lhs[0] != rhs[0] || lhs[1] != rhs[1];
}

// Lexicographic comparison
inline bool operator<(const Bond& lhs, const Bond& rhs) {
    return lhs[0] < rhs[0] || (lhs[0] == rhs[0] && lhs[1] < rhs[1]);
}

// Lexicographic comparison
inline bool operator>(const Bond& lhs, const Bond& rhs) {
    return lhs[0] > rhs[0] || (lhs[0] == rhs[0] && lhs[1] > rhs[1]);
}

/// The angle struct ensure a canonical representation of an angle between the
/// atoms i, j and k, with i < k
struct CHFL_EXPORT Angle {
    Angle(size_t first, size_t midle, size_t last) {
        if (first == midle || first == last || midle == last) {
            throw Error("Can not have the same atom twice in an angle");
        }

        data_[0] = std::min(first, last);
        data_[1] = midle;
        data_[2] = std::max(first, last);
    }

    /// Get the index of the `i`th atom (`i == 0`, `i == 1` or `i == 2`) in the
    /// angle.
    ///
    /// @throws OutOfBounds if `i` is not 0, 1 or 2
    const size_t& operator[](size_t i) const {
        if (i >= 3) {
            throw OutOfBounds("Can not access angle atom " + std::to_string(i));
        }
        return data_[i];
    }

    Angle(Angle&&) = default;
    Angle& operator=(Angle&&) = default;
    Angle(const Angle&) = default;
    Angle& operator=(const Angle&) = default;

private:
    std::array<size_t, 3> data_;
};

inline bool operator==(const Angle& lhs, const Angle& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];
}

inline bool operator!=(const Angle& lhs, const Angle& rhs) {
    return lhs[0] != rhs[0] || lhs[1] != rhs[1] || lhs[2] != rhs[2];
}

// Lexicographic comparison
inline bool operator<(const Angle& lhs, const Angle& rhs) {
    return lhs[0] < rhs[0] || (lhs[0] == rhs[0] && (
        lhs[1] < rhs[1] || (lhs[1] == rhs[1] && (lhs[2] < rhs[2]))
    ));
}

// Lexicographic comparison
inline bool operator>(const Angle& lhs, const Angle& rhs) {
    return lhs[0] > rhs[0] || (lhs[0] == rhs[0] && (
        lhs[1] > rhs[1] || (lhs[1] == rhs[1] && (lhs[2] > rhs[2]))
    ));
}

/// The dihedral struct ensure a canonical representation of a dihedral angle
/// between the atoms i, j, k and m, with max(i, j) < max(k, m))
struct CHFL_EXPORT Dihedral {
    Dihedral(size_t first, size_t second, size_t third, size_t fourth) {
        if (first == second || second == third || third == fourth) {
            throw Error("Can not have an atom linked to itself in a dihedral angle");
        }

        if (first == third || second == fourth || first == fourth) {
            throw Error("Can not have an atom twice in a dihedral angle");
        }

        if (std::max(first, second) < std::max(third, fourth)) {
            data_[0] = first;
            data_[1] = second;
            data_[2] = third;
            data_[3] = fourth;
        } else {
            data_[0] = fourth;
            data_[1] = third;
            data_[2] = second;
            data_[3] = first;
        }
    }

    /// Get the index of the `i`th atom (`i` can be 0, 1, 2 or 3) in the
    /// dihedral.
    ///
    /// @throws OutOfBounds if `i` is not 0, 1, 2 or 3.
    const size_t& operator[](size_t i) const {
        if (i >= 4) {
            throw OutOfBounds("Can not access dihedral atom " + std::to_string(i));
        }
        return data_[i];
    }

    Dihedral(Dihedral&&) = default;
    Dihedral& operator=(Dihedral&&) = default;
    Dihedral(const Dihedral&) = default;
    Dihedral& operator=(const Dihedral&) = default;

private:
    std::array<size_t, 4> data_;
};

inline bool operator==(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
}

inline bool operator!=(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs[0] != rhs[0] || lhs[1] != rhs[1] || lhs[2] != rhs[2] || lhs[3] != rhs[3];
}

// Lexicographic comparison
inline bool operator<(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs[0] < rhs[0] || (lhs[0] == rhs[0] && (
        lhs[1] < rhs[1] || (lhs[1] == rhs[1] && (
            lhs[2] < rhs[2] || (lhs[2] == rhs[2] && (lhs[3] < rhs[3]))
        ))
    ));
}

// Lexicographic comparison
inline bool operator>(const Dihedral& lhs, const Dihedral& rhs) {
    return lhs[0] > rhs[0] || (lhs[0] == rhs[0] && (
        lhs[1] > rhs[1] || (lhs[1] == rhs[1] && (
            lhs[2] > rhs[2] || (lhs[2] == rhs[2] && (lhs[3] > rhs[3]))
        ))
    ));
}

/// The connectivity struct store a cache of the bonds, angles and dihedrals
/// in the system. The `recalculate` function should be called when bonds are
/// added or removed. The `bonds` set is the main source of information, all the
/// other data are cached from it.
class Connectivity {
public:
    Connectivity() = default;
    /// Recalculate the angles and the dihedrals from the bond list
    void recalculate() const;
    /// Access the underlying data
    const sorted_set<Bond>& bonds() const;
    const sorted_set<Angle>& angles() const;
    const sorted_set<Dihedral>& dihedrals() const;
    /// Add a bond between the atoms `i` and `j`
    void add_bond(size_t i, size_t j);
    /// Remove any bond between the atoms `i` and `j`
    void remove_bond(size_t i, size_t j);

private:
    /// Biggest index within the atoms we know about. Used to pre-allocate
    /// memory when recomputing bonds.
    size_t biggest_atom_ = 0;
    /// Bonds in the system
    sorted_set<Bond> bonds_;
    /// Angles in the system
    mutable sorted_set<Angle> angles_;
    /// Dihedral angles in the system
    mutable sorted_set<Dihedral> dihedrals_;
    /// Is the cached content up to date ?
    mutable bool uptodate_ = false;
};


} // namespace chemfiles

#endif
