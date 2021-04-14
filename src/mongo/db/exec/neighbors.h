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

#pragma once

#include <queue>

#include "mongo/base/string_data.h"
#include "mongo/db/catalog/collection.h"
#include "mongo/db/exec/plan_stage.h"
#include "mongo/db/exec/plan_stats.h"
#include "mongo/db/exec/requires_index_stage.h"
#include "mongo/db/exec/working_set.h"
#include "mongo/db/matcher/expression.h"
#include "mongo/db/query/stage_types.h"
#include "mongo/db/record_id.h"
#include "mongo/stdx/unordered_map.h"


namespace mongo {

struct NeighborsStageParams {
    NeighborsStageParams() {}
    int k;
    std::vector<double> queryPoint;

    // MatchExpression* filter; // TODO filter is too complicated. don't allow it for now

    IndexBounds baseBounds;  // TODO whats this for?

    // TODO query point vector/record id?
    // const GeoNearExpression* nearQuery
    // TODO should this have addPointMeta/addDistMeta like GeoNearParams
};

// TODO this is an index_scan() like solution, is there a filter(colls_scan()) like solution?
class NeighborsStage : public RequiresIndexStage {
public:
    NeighborsStage(const NeighborsStageParams& params,
                   ExpressionContext* expCtx,
                   WorkingSet* workingSet,
                   const CollectionPtr& collection,
                   const IndexDescriptor* indexDescriptor);

    ~NeighborsStage();

    bool isEOF() final;
    StageState doWork(WorkingSetID* out) final;

    StageType stageType() const final;
    std::unique_ptr<PlanStageStats> getStats() final;
    const SpecificStats* getSpecificStats() const final;

protected:
    /*
     * Initialize near stage before buffering the data.
     * Return IS_EOF if subclass finishes the initialization.
     * Return NEED_TIME if we need more time.
     * Return errors if an error occurs.
     * Can't return ADVANCED.
     *
    TODO this method came from NearStage, is it needed?
    virtual StageState initialize(OperationContext* opCtx,
                                  WorkingSet* workingSet,
                                  WorkingSetID* out) = 0;
     */

    void doSaveStateRequiresIndex() final {}

    void doRestoreStateRequiresIndex() final {}

private:
    NeighborsStageParams _params;

    //
    // Generic state for progressive near search
    //

    WorkingSet* const _workingSet;

    // Tracks RecordIds from the child stage to do our own deduping.
    stdx::unordered_map<RecordId, WorkingSetID, RecordId::Hasher> _seenDocuments;

    // Stats for the stage covering this interval
    // This is owned by _specificStats
    IntervalStats* _nextIntervalStats;

    // Sorted buffered results to be returned - the current interval
    struct SearchResult {
        SearchResult(WorkingSetID resultID, double distance)
            : resultID(resultID), distance(distance) {}

        bool operator<(const SearchResult& other) const {
            // We want increasing distance, not decreasing, so we reverse the <
            return distance > other.distance;
        }

        WorkingSetID resultID;
        double distance;
    };
    std::priority_queue<SearchResult> _resultBuffer;

    // Stats

    // The current stage from which this stage should buffer results
    // Pointer to the last interval in _childrenIntervals. Owned by _childrenIntervals.
    // CoveredInterval* _nextInterval; // TODO do we need this/what is it

    // All children CoveredIntervals and the sub-stages owned by them.
    //
    // All children intervals except the last active one are only used by getStats(),
    // because they are all EOF.
    // TODO do we need this/what is it
    // std::vector<std::unique_ptr<CoveredInterval>> _childrenIntervals;
};

}  // namespace mongo
