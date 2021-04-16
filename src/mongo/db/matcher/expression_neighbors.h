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

#include <vector>

#include "mongo/db/matcher/expression.h"
#include "mongo/db/matcher/expression_leaf.h"

namespace mongo {

class NeighborsMatchExpression : public LeafMatchExpression {
public:
    NeighborsMatchExpression(clonable_ptr<ErrorAnnotation> annotation = nullptr);
    virtual ~NeighborsMatchExpression() {}

    bool matchesSingleElement(const BSONElement&, MatchDetails* details = nullptr) const final;

    virtual void debugString(StringBuilder& debug, int indentationLevel = 0) const;

    BSONObj getSerializedRightHandSide() const final;

    virtual bool equivalent(const MatchExpression* other) const;

    virtual std::unique_ptr<MatchExpression> shallowClone() const;

    void setCanSkipValidation(bool val) {
        _canSkipValidation = val;
    }

    bool getCanSkipValidation() const {
        return _canSkipValidation;
    }

    void acceptVisitor(MatchExpressionMutableVisitor* visitor) final {
        visitor->visit(this);
    }

    void acceptVisitor(MatchExpressionConstVisitor* visitor) const final {
        visitor->visit(this);
    }

    Status parseFrom(const BSONObj& obj);

    const std::vector<Decimal128>& getQueryPoint() const {
	return _queryPoint;
    }

    const int& getK() const {
	return _k;
    }

private:
    ExpressionOptimizerFunc getOptimizer() const final {
        return [](std::unique_ptr<MatchExpression> expression) { return expression; };
    }

    // The original geo specification provided by the user.
    BSONObj _rawObj;

    int _k = 1;
    std::vector<Decimal128> _queryPoint;

    // Share ownership of our query with all of our clones
    bool _canSkipValidation;
};
