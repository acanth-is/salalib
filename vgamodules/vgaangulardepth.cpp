// sala - a component of the depthmapX - spatial network analysis platform
// Copyright (C) 2000-2010, University College London, Alasdair Turner
// Copyright (C) 2011-2012, Tasos Varoudis
// Copyright (C) 2017-2024, Petros Koutsolampros

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

#include "salalib/vgamodules/vgaangulardepth.h"

AnalysisResult VGAAngularDepth::run(Communicator *,
                                    PointMap &map,
                                    bool) {

    AnalysisResult result;

    AttributeTable &attributes = map.getAttributeTable();

    // n.b., insert columns sets values to -1 if the column already exists
    std::string colText = "Angular Step Depth";
    int path_angle_col = attributes.insertOrResetColumn(colText);
    result.addAttribute(colText);

    for (auto iter = attributes.begin(); iter != attributes.end(); iter++) {
        PixelRef pix = iter->getKey().value;
        map.getPoint(pix).m_misc = 0;
        map.getPoint(pix).m_dist = 0.0f;
        map.getPoint(pix).m_cumangle = -1.0f;
    }

    std::set<AngularTriple> search_list; // contains root point

    for (auto &sel : map.getSelSet()) {
        search_list.insert(AngularTriple(0.0f, sel, NoPixel));
        map.getPoint(sel).m_cumangle = 0.0f;
    }

    // note that m_misc is used in a different manner to analyseGraph / PointDepth
    // here it marks the node as used in calculation only
    while (search_list.size()) {
        std::set<AngularTriple>::iterator it = search_list.begin();
        AngularTriple here = *it;
        search_list.erase(it);
        Point &p = map.getPoint(here.pixel);
        // nb, the filled check is necessary as diagonals seem to be stored with 'gaps' left in
        if (p.filled() && p.m_misc != ~0) {
            p.getNode().extractAngular(search_list, &map, here);
            p.m_misc = ~0;
            AttributeRow &row = map.getAttributeTable().getRow(AttributeKey(here.pixel));
            row.setValue(path_angle_col, float(p.m_cumangle));
            if (!p.getMergePixel().empty()) {
                Point &p2 = map.getPoint(p.getMergePixel());
                if (p2.m_misc != ~0) {
                    p2.m_cumangle = p.m_cumangle;
                    AttributeRow &mergePixelRow =
                        map.getAttributeTable().getRow(AttributeKey(p.getMergePixel()));
                    mergePixelRow.setValue(path_angle_col, float(p2.m_cumangle));
                    p2.getNode().extractAngular(
                        search_list, &map, AngularTriple(here.angle, p.getMergePixel(), NoPixel));
                    p2.m_misc = ~0;
                }
            }
        }
    }

    map.setDisplayedAttribute(-2);
    map.setDisplayedAttribute(path_angle_col);

    result.completed = true;

    return result;
}
