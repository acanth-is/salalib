// sala - a component of the depthmapX - spatial network analysis platform
// Copyright (C) 2011-2012, Tasos Varoudis
// Copyright (C) 2018 Petros Koutsolampros

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "genlib/p2dpoly.h"

struct NtfPoint {
    int m_chars;
    int a;
    int b;
    NtfPoint(int chars = 10) // apparently 10 is NTF default
    {
        m_chars = chars;
    }
    int parse(const std::string &token, bool secondhalf = false);
};

class NtfGeometry {
  public:
    std::vector<Line> lines;
    NtfGeometry() { ; }
};

class NtfLayer {
    friend class NtfMap;

  protected:
    std::string m_name;
    int m_line_count;

  public:
    int pad1 : 32;
    std::vector<NtfGeometry> geometries;
    NtfLayer(const std::string &name = std::string()) {
        m_name = name;
        m_line_count = 0;
    }
    int getLineCount() { return m_line_count; }
    std::string getName() { return m_name; }
};

class NtfMap {
  public:
    std::vector<NtfLayer> layers;
    enum { NTF_UNKNOWN, NTF_LANDLINE, NTF_MERIDIAN };

  protected:
    NtfPoint m_offset; // note: in metres
    QtRegion m_region; // made in metres, although points are in cm
    int m_line_count;

  public:
    NtfMap() { ; }
    Line makeLine(const NtfPoint &a, const NtfPoint &b);

    void open(const std::vector<std::string> &fileset, Communicator *comm);
    const QtRegion &getRegion() const { return m_region; }
    int getLineCount() const { return m_line_count; }

  protected:
    void fitBounds(const Line &li);
    void addGeom(size_t layerIdx, NtfGeometry &geom);
};
