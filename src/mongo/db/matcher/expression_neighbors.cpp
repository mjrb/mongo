/**
 *    Copyright (C) 2018-present MongoDB, Inc.
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

#define MONGO_LOGV2_DEFAULT_COMPONENT ::mongo::logv2::LogComponent::kDefault

#include "mongo/db/matcher/expression_neighbors.h"

#include "mongo/bson/simple_bsonobj_comparator.h"
#include "mongo/db/matcher/expression_parser.h"
#include "mongo/logv2/log.h"
#include "mongo/platform/basic.h"
#include "mongo/util/str.h"

namespace mongo {

/**
 * Takes shared ownership of the passed-in GeoExpression.
 */
NeighborsMatchExpression::NeighborsMatchExpression(clonable_ptr<ErrorAnnotation> annotation)
    : LeafMatchExpression(NEIGHBORS, "neighborssss"_sd, std::move(annotation)),
      _canSkipValidation(false) {}

bool NeighborsMatchExpression::matchesSingleElement(const BSONElement& e, MatchDetails* details) const {
    // TODO, uhhhhhh idk what to do for a bunch of these functions
    return false;
    /*
    if (!e.isABSONObj())
        return false;

    GeometryContainer geometry;

    if (!geometry.parseFromStorage(e, _canSkipValidation).isOK())
        return false;

    // Never match big polygon
    if (geometry.getNativeCRS() == STRICT_SPHERE)
        return false;

    // Project this geometry into the CRS of the query
    if (!geometry.supportsProject(_query->getGeometry().getNativeCRS()))
        return false;

    geometry.projectInto(_query->getGeometry().getNativeCRS());

    if (GeoExpression::WITHIN == _query->getPred()) {
        return _query->getGeometry().contains(geometry);
    } else {
        verify(GeoExpression::INTERSECT == _query->getPred());
        return _query->getGeometry().intersects(geometry);
    }
    */
}

void NeighborsMatchExpression::debugString(StringBuilder& debug, int indentationLevel) const {
    debug << "TODO serialize $neighbors\n";
    /*
    _debugAddSpace(debug, indentationLevel);

    BSONObjBuilder builder;
    serialize(&builder, true);
    debug << "GEO raw = " << builder.obj().toString();

    MatchExpression::TagData* td = getTag();
    if (nullptr != td) {
        debug << " ";
        td->debugString(&debug);
    }
    debug << "\n";
    */
}

BSONObj NeighborsMatchExpression::getSerializedRightHandSide() const {
    return BSONObj();
}

bool NeighborsMatchExpression::equivalent(const MatchExpression* other) const {
    return false; // TODO will this cause bugs???
    /*
    if (matchType() != other->matchType())
        return false;

    const GeoMatchExpression* realOther = static_cast<const GeoMatchExpression*>(other);

    if (path() != realOther->path())
        return false;

        return SimpleBSONObjComparator::kInstance.evaluate(_rawObj == realOther->_rawObj); */
}

std::unique_ptr<MatchExpression> NeighborsMatchExpression::shallowClone() const {
    std::unique_ptr<GeoMatchExpression> next =
        std::make_unique<NeighborsMatchExpression>(_errorAnnotation);
    next->_canSkipValidation = _canSkipValidation;
    if (getTag()) {
        next->setTag(getTag()->clone());
    }
    next->parseFrom(_rawObj);
    return next;
}

Status parseFrom(const BSONObj& obj) {
    _rawObj = obj.clone();
    BSONObjIterator boi(obj);
    while (boi.more()) {
        auto e = boi.next();
        if (e.fieldName() == "k") {
            _k = e.Int();
        } else if (e.feildName() == "point") {
            auto a = e.Array();
            for (auto feature : a) {
                _queryPoint.push_back(feautre.Decimal());
            }
        }
    }
    uassert(42000020, "point required in $neighbors", _queryPoint.size() > 0);
}

}  // namespace mongo
