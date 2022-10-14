/*
 * Copyright (c) 2020, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibJS/Runtime/Error.h>
#include <LibJS/Runtime/GlobalObject.h>
#include <LibJS/Runtime/RegExpConstructor.h>
#include <LibJS/Runtime/RegExpObject.h>
#include <LibJS/Runtime/Value.h>

namespace JS {

void RegExpStaticProperties::invalid()
{
    m_input = Utf16String(String::empty().view());
    m_last_match = Utf16String(String::empty().view());
    m_last_paren = Utf16String(String::empty().view());
    m_left_context = Utf16String(String::empty().view());
    m_right_context = Utf16String(String::empty().view());
    m_parens.clear();
}

RegExpConstructor::RegExpConstructor(Realm& realm)
    : NativeFunction(realm.vm().names.RegExp.as_string(), *realm.intrinsics().function_prototype())
{
}

void RegExpConstructor::initialize(Realm& realm)
{
    auto& vm = this->vm();
    NativeFunction::initialize(realm);

    // 22.2.4.1 RegExp.prototype, https://tc39.es/ecma262/#sec-regexp.prototype
    define_direct_property(vm.names.prototype, realm.intrinsics().regexp_prototype(), 0);

    define_native_accessor(realm, *vm.well_known_symbol_species(), symbol_species_getter, {}, Attribute::Configurable);

    define_direct_property(vm.names.length, Value(2), Attribute::Configurable);

    // Additional properties of the RegExp constructor, https://github.com/tc39/proposal-regexp-legacy-features#additional-properties-of-the-regexp-constructor
    define_native_accessor(realm, "input", input_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$_", input_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "lastMatch", last_match_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$&", last_match_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "lastParen", last_paren_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$+", last_paren_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "leftContext", left_context_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$`", left_context_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "rightContext", right_context_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$'", right_context_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$1", group_1_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$2", group_2_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$3", group_3_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$4", group_4_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$5", group_5_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$6", group_6_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$7", group_7_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$8", group_8_getter, {}, Attribute::Configurable);
    define_native_accessor(realm, "$9", group_9_getter, {}, Attribute::Configurable);
}

// UpdateLegacyRegExpStaticProperties ( C, S, startIndex, endIndex, capturedValues )
// https://github.com/tc39/proposal-regexp-legacy-features#updatelegacyregexpstaticproperties--c-s-startindex-endindex-capturedvalues-
void RegExpConstructor::update_legacy_properties(Utf16String const& string, size_t start_index, size_t end_index, Vector<Utf16View>& captured_values)
{
    // 1. Assert: C is an Object that has a [[RegExpInput]] internal slot.
    // 2. Assert: Type(S) is String.

    // 3. Let len be the number of code units in S.
    size_t len = string.length_in_code_units();

    // 4. Assert: startIndex and endIndex are integers such that 0 ≤ startIndex ≤ endIndex ≤ len.
    VERIFY(start_index <= end_index);
    VERIFY(end_index <= len);

    // 5. Assert: capturedValues is a List of Strings.

    // 6. Let n be the number of elements in capturedValues.
    size_t group_count = captured_values.size();

    // 7. Set the value of C’s [[RegExpInput]] internal slot to S.
    m_legacy_properties.set_input(string);

    // 8. Set the value of C’s [[RegExpLastMatch]] internal slot to a String whose length is endIndex - startIndex and containing the code units from S with indices startIndex through endIndex - 1, in ascending order.
    auto const& lastMatch = Utf16String(string.view().substring_view(start_index, end_index - start_index));
    m_legacy_properties.set_last_match(lastMatch);

    // 9. If n > 0, set the value of C’s [[RegExpLastParen]] internal slot to the last element of capturedValues.
    if (group_count > 0) {
        auto& item = captured_values[group_count - 1];
        m_legacy_properties.set_last_paren(Utf16String(item));
    }
    // 10. Else, set the value of C’s [[RegExpLastParen]] internal slot to the empty String.
    else {
        auto const& last_paren = Utf16String(String::empty().view());
        m_legacy_properties.set_last_paren(last_paren);
    }

    // 11. Set the value of C’s [[RegExpLeftContext]] internal slot to a String whose length is startIndex and containing the code units from S with indices 0 through startIndex - 1, in ascending order.
    auto const& left_context = Utf16String(string.view().substring_view(0, start_index));
    m_legacy_properties.set_left_context(left_context);

    // 12. Set the value of C’s [[RegExpRightContext]] internal slot to a String whose length is len - endIndex and containing the code units from S with indices endIndex through len - 1, in ascending order.
    auto const& right_context = Utf16String(string.view().substring_view(end_index, len - end_index));
    m_legacy_properties.set_right_context(right_context);

    // 13. For each integer i such that 1 ≤ i ≤ 9
    for (size_t i = 0; i < group_count; i++) {
        // If i ≤ n, set the value of C’s [[RegExpPareni]] internal slot to the ith element of capturedValues.
        auto& value = captured_values[i];
        m_legacy_properties.set_paren(i + 1, Utf16String(value));

        // Else, set the value of C’s [[RegExpPareni]] internal slot to the empty String.
        // NOTE: We don't need to perform the else branch because RegExpStaticProperties::paren_at will handle it.
    }
}

// 22.2.3.1 RegExp ( pattern, flags ), https://tc39.es/ecma262/#sec-regexp-pattern-flags
ThrowCompletionOr<Value> RegExpConstructor::call()
{
    auto& vm = this->vm();

    auto pattern = vm.argument(0);
    auto flags = vm.argument(1);

    bool pattern_is_regexp = TRY(pattern.is_regexp(vm));

    if (pattern_is_regexp && flags.is_undefined()) {
        auto pattern_constructor = TRY(pattern.as_object().get(vm.names.constructor));
        if (same_value(this, pattern_constructor))
            return pattern;
    }

    return TRY(construct(*this));
}

// 22.2.3.1 RegExp ( pattern, flags ), https://tc39.es/ecma262/#sec-regexp-pattern-flags
ThrowCompletionOr<Object*> RegExpConstructor::construct(FunctionObject& new_target)
{
    auto& vm = this->vm();

    auto pattern = vm.argument(0);
    auto flags = vm.argument(1);

    bool pattern_is_regexp = TRY(pattern.is_regexp(vm));

    Value pattern_value;
    Value flags_value;

    if (pattern.is_object() && is<RegExpObject>(pattern.as_object())) {
        auto& regexp_pattern = static_cast<RegExpObject&>(pattern.as_object());
        pattern_value = js_string(vm, regexp_pattern.pattern());

        if (flags.is_undefined())
            flags_value = js_string(vm, regexp_pattern.flags());
        else
            flags_value = flags;
    } else if (pattern_is_regexp) {
        pattern_value = TRY(pattern.as_object().get(vm.names.source));

        if (flags.is_undefined()) {
            flags_value = TRY(pattern.as_object().get(vm.names.flags));
        } else {
            flags_value = flags;
        }
    } else {
        pattern_value = pattern;
        flags_value = flags;
    }

    // https://github.com/tc39/proposal-regexp-legacy-features#regexpalloc--newtarget-
    auto* regexp_object = TRY(regexp_create(vm, pattern_value, flags_value));

    // 2. Let thisRealm be the current Realm Record.
    auto* realm = vm.current_realm();

    // FIXME: 3. Set the value of obj’s [[Realm]] internal slot to thisRealm.

    // 4. If SameValue(newTarget, thisRealm.[[Intrinsics]].[[%RegExp%]]) is true, then
    if (same_value(&new_target, realm->intrinsics().regexp_constructor())) {
        // i. Set the value of obj’s [[LegacyFeaturesEnabled]] internal slot to true.
        regexp_object->set_has_legacy_features(true);
    }
    // 5. Else,
    else {
        // i. Set the value of obj’s [[LegacyFeaturesEnabled]] internal slot to false.
        regexp_object->set_has_legacy_features(false);
    }

    return regexp_object;
}

// 22.2.4.2 get RegExp [ @@species ], https://tc39.es/ecma262/#sec-get-regexp-@@species
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::symbol_species_getter)
{
    return vm.this_value();
}

// get RegExp.input https://github.com/tc39/proposal-regexp-legacy-features#get-regexpinput
// get RegExp.$_ https://github.com/tc39/proposal-regexp-legacy-features#get-regexp_
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::input_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpInput]]).
    return js_string(vm, regexp_constructor->legacy_properties().input());
}

// get RegExp.lastMatch https://github.com/tc39/proposal-regexp-legacy-features#get-regexplastmatch
// get RegExp.$& https://github.com/tc39/proposal-regexp-legacy-features#get-regexp
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::last_match_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpLastMatch]]).
    return js_string(vm, regexp_constructor->legacy_properties().last_match());
}

// get RegExp.lastParen https://github.com/tc39/proposal-regexp-legacy-features#get-regexplastparen
// get RegExp.$+ https://github.com/tc39/proposal-regexp-legacy-features#get-regexp-1
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::last_paren_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpLastParen]]).
    return js_string(vm, regexp_constructor->legacy_properties().last_paren());
}

// get RegExp.leftContext https://github.com/tc39/proposal-regexp-legacy-features#get-regexplastparen
// get RegExp.$` https://github.com/tc39/proposal-regexp-legacy-features#get-regexp-2
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::left_context_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpLeftContext]]).
    return js_string(vm, regexp_constructor->legacy_properties().left_context());
}

// get RegExp.rightContext https://github.com/tc39/proposal-regexp-legacy-features#get-regexprightcontext
// get RegExp.$' https://github.com/tc39/proposal-regexp-legacy-features#get-regexp-3
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::right_context_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpRightContext]]).
    return js_string(vm, regexp_constructor->legacy_properties().right_context());
}

// get RegExp.$1 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp1
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_1_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen1]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(1));
}

// get RegExp.$2 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp2
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_2_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen2]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(2));
}

// get RegExp.$3 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp3
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_3_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen3]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(3));
}

// get RegExp.$4 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp4
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_4_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen4]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(4));
}

// get RegExp.$5 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp5
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_5_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen5]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(5));
}

// get RegExp.$6 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp6
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_6_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen6]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(6));
}

// get RegExp.$7 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp7
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_7_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen7]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(7));
}

// get RegExp.$8 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp8
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_8_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen8]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(8));
}

// get RegExp.$9 https://github.com/tc39/proposal-regexp-legacy-features#get-regexp9
JS_DEFINE_NATIVE_FUNCTION(RegExpConstructor::group_9_getter)
{
    auto* regexp_constructor = vm.current_realm()->intrinsics().regexp_constructor();

    // Return ? GetLegacyRegExpStaticProperty(%RegExp%, this value, [[RegExpParen9]]).
    return js_string(vm, regexp_constructor->legacy_properties().paren_at(9));
}
}
