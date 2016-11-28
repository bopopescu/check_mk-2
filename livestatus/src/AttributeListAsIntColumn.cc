// +------------------------------------------------------------------+
// |             ____ _               _        __  __ _  __           |
// |            / ___| |__   ___  ___| | __   |  \/  | |/ /           |
// |           | |   | '_ \ / _ \/ __| |/ /   | |\/| | ' /            |
// |           | |___| | | |  __/ (__|   <    | |  | | . \            |
// |            \____|_| |_|\___|\___|_|\_\___|_|  |_|_|\_\           |
// |                                                                  |
// | Copyright Mathias Kettner 2014             mk@mathias-kettner.de |
// +------------------------------------------------------------------+
//
// This file is part of Check_MK.
// The official homepage is at http://mathias-kettner.de/check_mk.
//
// check_mk is free software;  you can redistribute it and/or modify it
// under the  terms of the  GNU General Public License  as published by
// the Free Software Foundation in version 2.  check_mk is  distributed
// in the hope that it will be useful, but WITHOUT ANY WARRANTY;  with-
// out even the implied warranty of  MERCHANTABILITY  or  FITNESS FOR A
// PARTICULAR PURPOSE. See the  GNU General Public License for more de-
// tails. You should have  received  a copy of the  GNU  General Public
// License along with GNU Make; see the file  COPYING.  If  not,  write
// to the Free Software Foundation, Inc., 51 Franklin St,  Fifth Floor,
// Boston, MA 02110-1301 USA.

#include "AttributeListAsIntColumn.h"
#include "AttributeListColumn.h"
#include "IntFilter.h"

using std::string;

Filter *AttributeListAsIntColumn::createFilter(RelationalOperator relOp,
                                               const string &value) {
    return new IntFilter(this, relOp,
                         AttributeListColumn::refValueFor(value, logger()));
}

int32_t AttributeListAsIntColumn::getValue(void *row, contact * /*unused*/) {
    char *p = reinterpret_cast<char *>(shiftPointer(row));
    if (p == nullptr) {
        return 0;
    }
    auto ptr = reinterpret_cast<int *>(p + _offset);
    return *reinterpret_cast<int32_t *>(ptr);
}