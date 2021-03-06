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

#include "ContactGroupsColumn.h"
#include "Column.h"
#include "MonitoringCore.h"
#include "Renderer.h"
#include "Row.h"

using std::make_unique;
using std::string;
using std::unique_ptr;

void ContactGroupsColumn::output(Row row, RowRenderer &r,
                                 contact * /* auth_user */) {
    ListRenderer l(r);
    for (auto cgm = getData(row); cgm != nullptr; cgm = cgm->next) {
        l.output(string(cgm->group_ptr->group_name));
    }
}

unique_ptr<ListColumn::Contains> ContactGroupsColumn::makeContains(
    const string &name) {
    class ContainsContactGroup : public Contains {
    public:
        ContainsContactGroup(MonitoringCore::ContactGroup *element,
                             ContactGroupsColumn *column)
            : _element(element), _column(column) {}

        bool operator()(Row row) override {
            for (auto cgm = _column->getData(row); cgm != nullptr;
                 cgm = cgm->next) {
                // TODO(sp) Remove evil cast below.
                if (cgm->group_ptr ==
                    reinterpret_cast<contactgroup *>(_element)) {
                    return true;
                }
            }
            return false;
        }

    private:
        MonitoringCore::ContactGroup *const _element;
        ContactGroupsColumn *_column;
    };

    return make_unique<ContainsContactGroup>(_mc->find_contactgroup(name),
                                             this);
}

bool ContactGroupsColumn::isEmpty(Row row) { return getData(row) == nullptr; }

contactgroupsmember *ContactGroupsColumn::getData(Row row) {
    if (auto data = columnData<void>(row)) {
        return *offset_cast<contactgroupsmember *>(data, _offset);
    }
    return nullptr;
}
