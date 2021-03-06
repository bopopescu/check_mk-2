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

#include "TableServiceGroups.h"
#include <memory>
#include "Column.h"
#include "OffsetStringColumn.h"
#include "Query.h"
#include "ServiceListColumn.h"
#include "ServiceListStateColumn.h"
#include "auth.h"
#include "nagios.h"

using std::make_unique;
using std::string;

/* this might be a hack (accessing Nagios' internal structures.
Ethan: please help me here: how should this be code to be
portable? */
extern servicegroup *servicegroup_list;

TableServiceGroups::TableServiceGroups(MonitoringCore *mc) : Table(mc) {
    addColumns(this, "", -1);
}

string TableServiceGroups::name() const { return "servicegroups"; }

string TableServiceGroups::namePrefix() const { return "servicegroup_"; }

// static
void TableServiceGroups::addColumns(Table *table, const string &prefix,
                                    int indirect_offset) {
    table->addColumn(make_unique<OffsetStringColumn>(
        prefix + "name", "The name of the service group",
        DANGEROUS_OFFSETOF(servicegroup, group_name), indirect_offset, -1, -1));
    table->addColumn(make_unique<OffsetStringColumn>(
        prefix + "alias", "An alias of the service group",
        DANGEROUS_OFFSETOF(servicegroup, alias), indirect_offset, -1, -1));
    table->addColumn(make_unique<OffsetStringColumn>(
        prefix + "notes", "Optional additional notes about the service group",
        DANGEROUS_OFFSETOF(servicegroup, notes), indirect_offset, -1, -1));
    table->addColumn(make_unique<OffsetStringColumn>(
        prefix + "notes_url",
        "An optional URL to further notes on the service group",
        DANGEROUS_OFFSETOF(servicegroup, notes_url), indirect_offset, -1, -1));
    table->addColumn(make_unique<OffsetStringColumn>(
        prefix + "action_url",
        "An optional URL to custom notes or actions on the service group",
        DANGEROUS_OFFSETOF(servicegroup, action_url), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListColumn>(
        prefix + "members",
        "A list of all members of the service group as host/service pairs",
        true, true, 0, DANGEROUS_OFFSETOF(servicegroup, members),
        indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListColumn>(
        prefix + "members_with_state",
        "A list of all members of the service group with state and has_been_checked",
        true, true, 1, DANGEROUS_OFFSETOF(servicegroup, members),
        indirect_offset, -1, -1));

    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "worst_service_state",
        "The worst soft state of all of the groups services (OK <= WARN <= UNKNOWN <= CRIT)",
        ServiceListStateColumn::Type::worst_state,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services", "The total number of services in the group",
        ServiceListStateColumn::Type::num,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_ok",
        "The number of services in the group that are OK",
        ServiceListStateColumn::Type::num_ok,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_warn",
        "The number of services in the group that are WARN",
        ServiceListStateColumn::Type::num_warn,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_crit",
        "The number of services in the group that are CRIT",
        ServiceListStateColumn::Type::num_crit,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_unknown",
        "The number of services in the group that are UNKNOWN",
        ServiceListStateColumn::Type::num_unknown,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_pending",
        "The number of services in the group that are PENDING",
        ServiceListStateColumn::Type::num_pending,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_ok",
        "The number of services in the group that are OK",
        ServiceListStateColumn::Type::num_hard_ok,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_warn",
        "The number of services in the group that are WARN",
        ServiceListStateColumn::Type::num_hard_warn,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_crit",
        "The number of services in the group that are CRIT",
        ServiceListStateColumn::Type::num_hard_crit,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
    table->addColumn(make_unique<ServiceListStateColumn>(
        prefix + "num_services_hard_unknown",
        "The number of services in the group that are UNKNOWN",
        ServiceListStateColumn::Type::num_hard_unknown,
        DANGEROUS_OFFSETOF(servicegroup, members), indirect_offset, -1, -1));
}

void TableServiceGroups::answerQuery(Query *query) {
    for (servicegroup *sg = servicegroup_list; sg != nullptr; sg = sg->next) {
        if (!query->processDataset(Row(sg))) {
            break;
        }
    }
}

Row TableServiceGroups::findObject(const string &objectspec) {
    return Row(find_servicegroup(const_cast<char *>(objectspec.c_str())));
}

bool TableServiceGroups::isAuthorized(Row row, contact *ctc) {
    if (ctc == unknown_auth_user()) {
        return false;
    }

    auto has_contact = [=](servicesmember *mem) {
        service *svc = mem->service_ptr;
        return is_authorized_for(ctc, svc->host_ptr, svc);
    };
    if (g_group_authorization == AuthorizationKind::loose) {
        // TODO(sp) Need an iterator here, "loose" means "any_of"
        for (servicesmember *mem = rowData<servicegroup>(row)->members;
             mem != nullptr; mem = mem->next) {
            if (has_contact(mem)) {
                return true;
            }
        }
        return false;
    }
    // TODO(sp) Need an iterator here, "strict" means "all_of"
    for (servicesmember *mem = rowData<servicegroup>(row)->members;
         mem != nullptr; mem = mem->next) {
        if (!has_contact(mem)) {
            return false;
        }
    }
    return true;
}
