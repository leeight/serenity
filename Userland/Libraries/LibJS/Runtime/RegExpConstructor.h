/*
 * Copyright (c) 2020, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/HashMap.h>
#include <AK/Utf16View.h>
#include <LibJS/Runtime/NativeFunction.h>

namespace JS {

static Utf16String empty_string(String::empty().view());

// https://github.com/tc39/proposal-regexp-legacy-features#regexp
// The %RegExp% instrinsic object, which is the builtin RegExp constructor, has the following additional internal slots:
// [[RegExpInput]]
// [[RegExpLastMatch]]
// [[RegExpLastParen]]
// [[RegExpLeftContext]]
// [[RegExpRightContext]]
// [[RegExpParen1]] ... [[RegExpParen9]]
class RegExpStaticProperties {
public:
    Utf16String& input() { return m_input; }
    Utf16String& last_match() { return m_last_match; }
    Utf16String& last_paren() { return m_last_paren; }
    Utf16String& left_context() { return m_left_context; }
    Utf16String& right_context() { return m_right_context; }
    HashMap<size_t, Utf16String>& parens() { return m_parens; }
    Utf16String& paren_at(size_t index)
    {
        if (m_parens.contains(index)) {
            return m_parens.get(index).value();
        }
        return empty_string;
    }
    void set_input(Utf16String const& input) { m_input = input; }
    void set_last_match(Utf16String const& last_match) { m_last_match = last_match; }
    void set_last_paren(Utf16String const& last_paren) { m_last_paren = last_paren; }
    void set_left_context(Utf16String const& left_context) { m_left_context = left_context; }
    void set_right_context(Utf16String const& right_context) { m_right_context = right_context; }
    void set_paren(size_t index, Utf16String const& value) { m_parens.set(index, value); }
    void invalid();

private:
    Utf16String m_input;
    Utf16String m_last_match;
    Utf16String m_last_paren;
    Utf16String m_left_context;
    Utf16String m_right_context;
    HashMap<size_t, Utf16String> m_parens;
};

class RegExpConstructor final : public NativeFunction {
    JS_OBJECT(RegExpConstructor, NativeFunction);

public:
    virtual void initialize(Realm&) override;
    virtual ~RegExpConstructor() override = default;

    virtual ThrowCompletionOr<Value> call() override;
    virtual ThrowCompletionOr<Object*> construct(FunctionObject& new_target) override;

    RegExpStaticProperties& legacy_properties() { return m_legacy_properties; }
    void update_legacy_properties(Utf16String const&, size_t, size_t, Vector<Utf16View>&);
    void reset_legacy_properties() { m_legacy_properties.invalid(); }

private:
    explicit RegExpConstructor(Realm&);

    virtual bool has_constructor() const override { return true; }

    JS_DECLARE_NATIVE_FUNCTION(symbol_species_getter);
    JS_DECLARE_NATIVE_FUNCTION(input_getter);
    JS_DECLARE_NATIVE_FUNCTION(last_match_getter);
    JS_DECLARE_NATIVE_FUNCTION(last_paren_getter);
    JS_DECLARE_NATIVE_FUNCTION(left_context_getter);
    JS_DECLARE_NATIVE_FUNCTION(right_context_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_1_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_2_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_3_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_4_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_5_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_6_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_7_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_8_getter);
    JS_DECLARE_NATIVE_FUNCTION(group_9_getter);

    RegExpStaticProperties m_legacy_properties;
};
}
