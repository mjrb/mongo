/**
 *    Copyright (C) 2021-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#include "mongo/platform/basic.h"

#include <memory>

#include "mongo/base/string_data.h"
#include "mongo/db/exec/neighbors.h"
#include "mongo/db/exec/working_set_common.h"

namespace mongo {

static const std::string kNDIndexNeighborsStage("ND_NEIGHBORS");

NeighborsStage::NeighborsStage(const NeighborsStageParams& params,
                               ExpressionContext* expCtx,
                               WorkingSet* workingSet,
                               const CollectionPtr& collection,
                               const IndexDescriptor* indexDescriptor)
    : RequiresIndexStage(
          kNDIndexNeighborsStage.c_str(), expCtx, collection, indexDescriptor, workingSet),
      _params(params),
      _workingSet(workingSet) {}

NeighborsStage::~NeighborsStage() {}

bool NeighborsStage::isEOF() {
    return true;  // TODO change this to actual behavior
};

PlanStage::StageState NeighborsStage::doWork(WorkingSetID* out) {
    // see near.cpp's doWork for some potential inspiration
    // the behavior is going to be a little different though
    // TODO change this to actual behavior

    return PlanStage::IS_EOF;
}

StageType NeighborsStage::stageType() const {
    return STAGE_NEIGHBORS;
}

std::unique_ptr<PlanStageStats> NeighborsStage::getStats() {
    std::unique_ptr<PlanStageStats> ret =
        std::make_unique<PlanStageStats>(_commonStats, STAGE_NEIGHBORS);
    // ret->specific = _specificStats.clone(); // TODO specific stats
    // TODO child stats?
    return ret;
}

const SpecificStats* NeighborsStage::getSpecificStats() const {
    // TODO specific stats. see near.cpp/near.h
    // return &_specificStats
    return nullptr;
}


}  // namespace mongo
