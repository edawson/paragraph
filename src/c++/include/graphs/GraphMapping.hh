// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Copyright (c) 2017 Illumina, Inc.
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <initializer_list>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "common/Error.hh"
#include "graphs/Graph.hh"

namespace graphs
{

class Operation
{
public:
    enum class Type
    {
        kMatch,
        kMismatch,
        kInsertionToRef,
        kDeletionFromRef,
        kSoftClipping,
        kMissingBases
    };

    Operation(char type_encoding, int length, std::string query, std::string reference)
    {
        decodeOperation(type_encoding, length, std::move(query), std::move(reference));
        validate();
    }
    Operation(std::string cigar, std::string query, std::string reference);
    Type type() const { return type_; }
    int length() const { return length_; }
    std::string query() const { return query_; }
    std::string reference() const { return reference_; }
    int32_t querySpan() const;
    int32_t referenceSpan() const;
    bool operator==(const Operation& other) const
    {
        return type_ == other.type_ && length_ == other.length_ && query_ == other.query_
            && reference_ == other.reference_;
    }
    char asSymbol() const;

private:
    void decodeOperation(char op_char, int length, std::string query_seq, std::string reference_seq);
    void validate() const;
    Type type_;
    int length_;
    std::string query_;
    std::string reference_;
};

std::ostream& operator<<(std::ostream& os, const Operation& operation);

class Mapping
{
public:
    typedef size_t size_type;

    Mapping() = default;

    Mapping(
        int32_t reference_start, std::vector<Operation> operations, const std::string& query,
        const std::string& reference)
        : operations_(std::move(operations))
    {
        updateCounts();
    }
    Mapping(
        int32_t reference_start, const std::string& encoding, const std::string& query, const std::string& reference);
    size_type num_operations() const { return operations_.size(); }
    std::string query() const;
    std::string reference() const;
    int32_t querySpan() const;
    int32_t referenceSpan() const;
    const Operation& operator[](size_type index) const { return operations_[index]; }
    bool operator==(const Mapping& other) const { return operations_ == other.operations_; }
    size_t matched() const { return matched_; }
    size_t mismatched() const { return mismatched_; }
    size_t clipped() const { return clipped_; }
    size_t inserted() const { return inserted_; }
    size_t deleted() const { return deleted_; }

protected:
    void decodeOperations(
        int32_t reference_start, const std::string& encoding, const std::string& query, const std::string& reference);

    void updateCounts();

private:
    size_t matched_ = 0;
    size_t mismatched_ = 0;
    size_t clipped_ = 0;
    size_t inserted_ = 0;
    size_t deleted_ = 0;
    size_t missing_ = 0;
    std::vector<Operation> operations_;
};

std::ostream& operator<<(std::ostream& os, const Mapping& mapping);

class NodeMapping : public Mapping
{
public:
    NodeMapping(int32_t reference_start, const std::string& node_cigar, const std::string& query, const Graph& graph);
    uint64_t node_id() const { return node_id_; }
    bool operator==(const NodeMapping& other) const
    {
        return node_id_ == other.node_id_ && static_cast<const Mapping&>(*this) == static_cast<const Mapping&>(other);
    }

private:
    uint64_t node_id_;
};

std::ostream& operator<<(std::ostream& os, const NodeMapping& cigar);

class GraphMapping
{
public:
    typedef size_t size_type;
    GraphMapping(
        int32_t first_node_reference_start, const std::string& graph_cigar, const std::string& query,
        const Graph& graph);
    std::string query() const;
    std::string reference() const;
    int32_t querySpan() const;
    int32_t referenceSpan() const;
    int32_t referenceStart() const { return first_node_reference_start_; }
    size_t size() const { return node_mappings_.size(); }
    const NodeMapping& operator[](size_type index) const { return node_mappings_[index]; }

private:
    int32_t first_node_reference_start_;
    std::vector<NodeMapping> node_mappings_;
};
}