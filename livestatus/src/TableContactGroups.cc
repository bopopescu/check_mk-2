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

#include "TableContactGroups.h"
#include <memory>
#include "Column.h"
#include "ContactGroupsMemberColumn.h"
#include "MonitoringCore.h"
#include "OffsetStringColumn.h"
#include "Query.h"
#include "nagios.h"

using std::make_unique;
using std::string;

extern contactgroup *contactgroup_list;

TableContactGroups::TableContactGroups(MonitoringCore *mc) : Table(mc) {
    addColumn(make_unique<OffsetStringColumn>(
        "name", "The name of the contactgroup",
        DANGEROUS_OFFSETOF(contactgroup, group_name), -1, -1, -1));
    addColumn(make_unique<OffsetStringColumn>(
        "alias", "The alias of the contactgroup",
        DANGEROUS_OFFSETOF(contactgroup, alias), -1, -1, -1));
    addColumn(make_unique<ContactGroupsMemberColumn>(
        "members", "A list of all members of this contactgroup", -1, -1, -1));
}

string TableContactGroups::name() const { return "contactgroups"; }

string TableContactGroups::namePrefix() const { return "contactgroup_"; }

void TableContactGroups::answerQuery(Query *query) {
    for (contactgroup *cg = contactgroup_list; cg != nullptr; cg = cg->next) {
        if (!query->processDataset(Row(cg))) {
            break;
        }
    }
}

Row TableContactGroups::findObject(const string &objectspec) {
    return Row(core()->find_contactgroup(objectspec));
}
