// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license

#include <fstream>
#include "catch.hpp"
#include "helpers.hpp"
#include "chemfiles.hpp"
using namespace chemfiles;

template <typename T>
static bool contains(const std::vector<T> haystack, const T& needle) {
    return std::find(haystack.begin(), haystack.end(), needle) != haystack.end();
}

TEST_CASE("Read files in PDB format"){
    SECTION("Read next step") {
        Trajectory file("data/pdb/water.pdb");
        Frame frame = file.read();

        CHECK(frame.natoms() == 297);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], vector3d(0.417, 8.303, 11.737), 1e-3));
        CHECK(approx_eq(positions[296], vector3d(6.664, 11.6148, 12.961), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);

        file.read(); // Skip a frame
        frame = file.read();

        CHECK(frame.natoms() == 297);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], vector3d(0.299, 8.310, 11.721), 1e-4));
        CHECK(approx_eq(positions[296], vector3d(6.798, 11.509, 12.704), 1e-4));
    }

    SECTION("Read a specific step"){
        Trajectory file("data/pdb/water.pdb");

        auto frame = file.read_step(2);
        CHECK(frame.natoms() == 297);
        auto positions = frame.positions();
        CHECK(approx_eq(positions[0], vector3d(0.299, 8.310, 11.721), 1e-4));
        CHECK(approx_eq(positions[296], vector3d(6.798, 11.509, 12.704), 1e-4));

        frame = file.read_step(0);
        CHECK(frame.natoms() == 297);
        positions = frame.positions();
        CHECK(approx_eq(positions[0], vector3d(0.417, 8.303, 11.737), 1e-3));
        CHECK(approx_eq(positions[296], vector3d(6.664, 11.6148, 12.961), 1e-3));

        auto cell = frame.cell();
        CHECK(cell.shape() == UnitCell::ORTHORHOMBIC);
        CHECK(fabs(cell.a() - 15.0) < 1e-5);
    }

    SECTION("Read bonds") {
        Trajectory file("data/pdb/MOF-5.pdb");
        Frame frame = file.read();

        auto topology = frame.topology();

        CHECK(topology.natoms() == 65);

        CHECK(topology[0].type() == "Zn");
        CHECK(topology[1].type() == "O");

        CHECK(topology[0].name() == "ZN");
        CHECK(topology[1].name() == "O");

        CHECK(topology.bonds().size() == 68);

        CHECK(contains(topology.bonds(), Bond(9, 38)));
        CHECK(contains(topology.bonds(), Bond(58, 62)));
        CHECK(contains(topology.bonds(), Bond(37, 24)));
        CHECK(contains(topology.bonds(), Bond(27, 31)));

        CHECK(contains(topology.angles(), Angle(20, 21, 23)));
        CHECK(contains(topology.angles(), Angle(9, 38, 44)));

        CHECK(contains(topology.dihedrals(), Dihedral(64, 62, 58, 53)));
        CHECK(contains(topology.dihedrals(), Dihedral(22, 21, 23, 33)));
    }

    SECTION("Support short records") {
        Trajectory file("data/pdb/cryst1.pdb");
        Frame frame = file.read();
    }

    SECTION("Read residue information") {
        Trajectory file("data/pdb/water.pdb");
        Frame frame = file.read();

        CHECK(frame.topology().residues().size() == 99);

        REQUIRE(frame.topology().residue(1));
        auto residue = (*frame.topology().residue(1));
        CHECK(residue.size() == 3);
        CHECK(residue.contains(0));
        CHECK(residue.contains(1));
        CHECK(residue.contains(2));

        file = Trajectory("data/pdb/MOF-5.pdb");
        frame = file.read();

        CHECK(frame.topology().residues().size() == 1);
        residue = frame.topology().residues()[0];
        CHECK(residue.size() == frame.natoms());
        CHECK(residue.name() == "LIG");
    }
}

TEST_CASE("Write files in PDB format") {
    auto tmpfile = NamedTempPath(".pdb");
    const auto EXPECTED_CONTENT =
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "HETATM    1    A RES X   1       1.000   2.000   3.000  1.00  0.00           A\n"
    "HETATM    2    B RES X   2       1.000   2.000   3.000  1.00  0.00           B\n"
    "HETATM    3    C RES X   3       1.000   2.000   3.000  1.00  0.00           C\n"
    "HETATM    4    D RES X   4       1.000   2.000   3.000  1.00  0.00           D\n"
    "CONECT    1    2\n"
    "CONECT    2    1\n"
    "END\n"
    "CRYST1   22.000   22.000   22.000  90.00  90.00  90.00 P 1           1\n"
    "HETATM    1    A RES X   4       4.000   5.000   6.000  1.00  0.00           A\n"
    "HETATM    2    B foo X   3       4.000   5.000   6.000  1.00  0.00           B\n"
    "HETATM    3    C foo X   3       4.000   5.000   6.000  1.00  0.00           C\n"
    "HETATM    4    D bar X  -1       4.000   5.000   6.000  1.00  0.00           D\n"
    "HETATM    5    E RES X   5       4.000   5.000   6.000  1.00  0.00           E\n"
    "HETATM    6    F RES X   6       4.000   5.000   6.000  1.00  0.00           F\n"
    "HETATM    7    G RES X   7       4.000   5.000   6.000  1.00  0.00           G\n"
    "CONECT    1    2    7\n"
    "CONECT    2    1    7\n"
    "CONECT    3    7\n"
    "CONECT    4    7\n"
    "CONECT    5    6    7\n"
    "CONECT    6    5    7\n"
    "CONECT    7    1    2    3    4\n"
    "CONECT    7    5    6\n"
    "END\n";

    Topology topology;
    topology.append(Atom("A"));
    topology.append(Atom("B"));
    topology.append(Atom("C"));
    topology.append(Atom("D"));
    topology.add_bond(0, 1);

    Frame frame(topology);
    frame.set_cell(UnitCell(22));

    auto positions = frame.positions();
    for(size_t i=0; i<4; i++) {
        positions[i] = vector3d(1, 2, 3);
    }

    {
        auto file = Trajectory(tmpfile, 'w');
        file.write(frame);
    }

    frame.resize(7);
    positions = frame.positions();
    for(size_t i=0; i<7; i++)
        positions[i] = vector3d(4, 5, 6);

    topology.append(Atom("E"));
    topology.append(Atom("F"));
    topology.append(Atom("G"));
    topology.add_bond(4, 5);
    topology.add_bond(0, 6);
    topology.add_bond(1, 6);
    topology.add_bond(2, 6);
    topology.add_bond(3, 6);
    topology.add_bond(4, 6);
    topology.add_bond(5, 6);

    Residue residue("foo", 3);
    residue.add_atom(1);
    residue.add_atom(2);
    topology.add_residue(residue);

    residue = Residue("barbar"); // This will be truncated in output
    residue.add_atom(3);
    topology.add_residue(residue);

    frame.set_topology(topology);

    {
        auto file = Trajectory(tmpfile, 'a');
        file.write(frame);
    }

    std::ifstream checking(tmpfile);
    std::string content((std::istreambuf_iterator<char>(checking)),
                         std::istreambuf_iterator<char>());
    CHECK(content == EXPECTED_CONTENT);
}

TEST_CASE("PDB files with big values") {
    auto tmpfile = NamedTempPath(".pdb");
    auto trajectory = Trajectory(tmpfile, 'w');

    auto frame = Frame(1);
    frame.set_cell(UnitCell(1234567890));
    CHECK_THROWS_AS(trajectory.write(frame), FormatError);

    frame.set_cell(UnitCell(12));
    frame.positions()[0] = vector3d(123456789, 2, 3);
    CHECK_THROWS_AS(trajectory.write(frame), FormatError);
}
