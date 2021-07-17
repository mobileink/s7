#!/bin/zsh

## https://blog.mimacom.com/arrays-on-linux-shell/
## https://gist.github.com/cinhtau/ea0bed82dfa8f996c3cd4149c2c31672

# echo "detecting shell:"
# if [[ -n ${ZSH_VERSION} ]] && [[ ! -z ${ZSH_VERSION} ]]; then
INDEX_START=1
INDEX_OFFSET=0
#   echo "  using zsh $ZSH_VERSION, index starts at $INDEX_START"
# elif [[ -n $BASH_VERSION ]]; then
#   INDEX_START=0
#   INDEX_OFFSET=1
#   echo "  using bash $BASH_VERSION, index starts at $INDEX_START"
# else
#   echo "  unknown"
# fi
# echo

MAN3DIR="`dirname $0:A`/man3"
echo "MAN3DIR: $MAN3DIR"

s7_booleans=(s7_f s7_t s7_is_boolean s7_boolean s7_make_boolean)
for i in "${s7_booleans[@]}"; do
    echo ${HOME}/man/man3/$i.3
    ln -s $MAN3DIR/s7_booleans.3 ${HOME}/man/man3/$i.3
done

s7_c_copy=(s7_copy)
for i in "${s7_c_copy[@]}"; do
    echo ${HOME}/man/man3/$i.3
    ln -s $MAN3DIR/s7_c_copy.3 ${HOME}/man/man3/$i.3
done

s7_c_equality=(
    s7_c_equality

    s7_is_eq s7_is_eqv s7_is_equal s7_is_equivalent)
for i in "${s7_c_equality[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_equality.3 ${HOME}/man/man3/$i.3
done

s7_c_errors=(
    s7_c_errors
    s7_error
    s7_wrong_type_arg_error
    s7_out_of_range_error
    s7_wrong_number_of_args_error
)

for i in "${s7_c_errors[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_errors.3 ${HOME}/man/man3/$i.3
done

s7_c_objects=(
    s7_c_objects

    s7_is_c_object
    s7_c_object_type
    s7_c_object_value
    s7_c_object_value_checked
    s7_make_c_object
    s7_make_c_object_with_let
    s7_make_c_object_without_gc
)
for i in "${s7_c_objects[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_objects.3 ${HOME}/man/man3/$i.3
done

s7_c_pointers=(
    s7_c_pointers

    s7_is_c_pointer
    s7_is_c_pointer_of_type
    s7_c_pointer
    s7_c_pointer_with_type
    s7_c_pointer_type
    s7_make_c_pointer
    s7_make_c_pointer_with_type
)
for i in "${s7_c_pointers[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_pointers.3 ${HOME}/man/man3/$i.3
done

s7_c_types=(
    s7_c_types

    s7_int
    s7_pointer
    s7_make_c_type
    s7_c_type_set_free
    s7_c_type_set_mark
    s7_c_type_set_gc_free
    s7_c_type_set_gc_mark
    s7_c_type_set_ref
    s7_c_type_set_set
    s7_c_type_set_length
    s7_c_type_set_copy
    s7_c_type_set_fill
    s7_c_type_set_reverse
    s7_c_type_set_to_list
    s7_c_type_set_to_string
    s7_c_type_set_getter
    s7_c_type_set_setter
)
for i in "${s7_c_types[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_types.3 ${HOME}/man/man3/$i.3
done

s7_characters=(
    s7_characters
    s7_is_character
    s7_character
    s7_make_character
)
for i in "${s7_characters[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_characters.3 ${HOME}/man/man3/$i.3
done

# s7_datatypes=(
# )

s7_c_debug=(
    s7_c_debug

    s7_stacktrace
)
for i in "${s7_c_debug[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_debug.3 ${HOME}/man/man3/$i.3
done

s7_define=(
    s7_define
    s7_is_defined
)
for i in "${s7_define[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_define.3 ${HOME}/man/man3/$i.3
done

s7_c_functions=(
    s7_c_functions

    s7_function
    s7_define_function
    s7_define_safe_function
    s7_define_typed_function
    s7_define_unsafe_typed_function
    s7_define_semisafe_typed_function
    s7_define_function_star
    s7_define_safe_function_star
    s7_define_typed_function_star
    s7_make_function
    s7_make_safe_function
    s7_make_typed_function
    s7_make_function_star
    s7_make_safe_function_star
)
for i in "${s7_c_functions[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_functions.3 ${HOME}/man/man3/$i.3
done

s7_c_constants=(
    s7_c_constants

    s7_define_constant
    s7_define_constant_with_documentation
    s7_define_constant_with_environment
)
for i in "${s7_c_constants[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_constants.3 ${HOME}/man/man3/$i.3
done

s7_c_variables=(
    s7_c_variables

    s7_define_variable
    s7_define_variable_with_documentation
)
for i in "${s7_c_variables[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_variables.3 ${HOME}/man/man3/$i.3
done

s7_c_dilambda=(
    s7_c_dilambda

    s7_dilambda
    s7_is_dilambda
    s7_typed_dilambda
    s7_dilambda_with_environment
)
for i in "${s7_c_dilambda[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_dilambda.3 ${HOME}/man/man3/$i.3
done

s7_c_lets=(
    s7_c_lets

    s7_rootlet
    s7_shadow_rootlet
    s7_set_shadow_rootlet
    s7_curlet
    s7_set_curlet
    s7_outlet
    s7_inlet
    s7_sublet
    s7_varlet
    s7_let_to_list
    s7_is_let
    s7_let_ref
    s7_let_set
    s7_openlet
    s7_is_openlet
    s7_method
    s7_let_field_ref
    s7_let_field_set
    s7_c_object_let
    s7_c_object_set_let
    s7_funclet
)
for i in "${s7_c_lets[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_lets.3 ${HOME}/man/man3/$i.3
done

s7_c_environments=( # i.e. symbol table access:
    s7_c_environments

    s7_name_to_value
    s7_symbol_table_find_name
    s7_symbol_value
    s7_symbol_set_value
    s7_symbol_local_value
    s7_for_each_symbol_name
    s7_for_each_symbol
)
for i in "${s7_c_environments[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_environments.3 ${HOME}/man/man3/$i.3
done

s7_c_eval=(
    s7_c_eval

    s7_eval_c_string
    s7_eval_c_string_with_environment
)
for i in "${s7_c_eval[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_eval.3 ${HOME}/man/man3/$i.3
done

s7_c_features=(
    s7_c_features

    s7_provide
    s7_is_provided
)
for i in "${s7_c_features[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_features.3 ${HOME}/man/man3/$i.3
done

s7_ffi_equality=(
    s7_ffi_equality

    s7_c_type_set_is_equal
    s7_c_type_set_equal
    s7_c_type_set_is_equivalent
)
for i in "${s7_ffi_equality[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_ffi_equality.3 ${HOME}/man/man3/$i.3
done

s7_format=(
    s7_format
)
for i in "${s7_format[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_format.3 ${HOME}/man/man3/$i.3
done

s7_c_hash_tables=(
    s7_c_hash_tables

    s7_is_hash_table
    s7_make_hash_table
    s7_hash_table_ref
    s7_hash_table_set
    s7_hash_code
)
for i in "${s7_c_hash_tables[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_hash_tables.3 ${HOME}/man/man3/$i.3
done

s7_c_repl=(
    s7_c_repl

    s7_history
    s7_add_to_history
    s7_history_enabled
    s7_set_history_enabled
)
for i in "${s7_c_repl[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_repl.3 ${HOME}/man/man3/$i.3
done

s7_c_hooks=(
    s7_c_hooks

    s7_begin_hook
    s7_set_begin_hook
)
for i in "${s7_c_hooks[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_hooks.3 ${HOME}/man/man3/$i.3
done

s7_c_io=(
    s7_c_io

    s7_open_input_file
    s7_open_output_file
    s7_open_input_string
    s7_open_output_string
    s7_get_output_string
    s7_open_output_function
    s7_open_input_function
    s7_read_char
    s7_peek_char
    s7_read
    s7_newline
    s7_write_char
    s7_write
    s7_display
)
for i in "${s7_c_io[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_io.3 ${HOME}/man/man3/$i.3
done

s7_c_ports=(
    s7_c_ports

    s7_is_input_port
    s7_is_output_port
    s7_port_filename
    s7_port_line_number
    s7_current_input_port
    s7_set_current_input_port
    s7_current_output_port
    s7_set_current_output_port
    s7_current_error_port
    s7_set_current_error_port
    s7_close_input_port
    s7_close_output_port
    s7_flush_output_port
)
for i in "${s7_c_ports[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_ports.3 ${HOME}/man/man3/$i.3
done

s7_iteration=(
    s7_iteration

    s7_scm_make-iterator
    s7_scm_iterator
    s7_scm_iterate
    s7_scm_iterator-sequence
    s7_scm_iterator-at-end
)
for i in "${s7_iteration[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_iteration.3 ${HOME}/man/man3/$i.3
done

s7_c_iterators=(
    s7_c_iterators

    s7_make_iterator
    s7_is_iterator
    s7_iterator_is_at_end
    s7_iterate
)
for i in "${s7_c_iterators[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_iterators.3 ${HOME}/man/man3/$i.3
done

s7_c_keywords=(
    s7_c_keywords

    s7_is_keyword
    s7_make_keyword
    s7_keyword_to_symbol
)
for i in "${s7_c_keywords[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_keywords.3 ${HOME}/man/man3/$i.3
done

s7_c_lists=(
    s7_c_lists

    s7_is_list
    s7_is_proper_list
    s7_list_length
    s7_make_list
    s7_list
    s7_list_nl
    s7_reverse
    s7_append
    s7_list_ref
    s7_list_set
    s7_assoc
    s7_assq
    s7_member
    s7_memq
    s7_tree_memq
)
for i in "${s7_c_lists[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_lists.3 ${HOME}/man/man3/$i.3
done

s7_c_load=(
    s7_c_load

    s7_load
    s7_load_with_environment
    s7_load_c_string
    s7_load_c_string_with_environment
    s7_load_path
    s7_add_to_load_path
    s7_autoload
    s7_autoload_set_names
)
for i in "${s7_c_load[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_load.3 ${HOME}/man/man3/$i.3
done

s7_c_macros=(
    s7_c_macros

    s7_is_macro
    s7_define_macro
)
for i in "${s7_c_macros[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_macros.3 ${HOME}/man/man3/$i.3
done

s7_c_memory=(
    s7_c_memory

    s7_init
    s7_free
    s7_gc_on
    s7_gc_protect
    s7_gc_unprotect_at
    s7_gc_protected_at
    s7_gc_protect_via_stack
    s7_gc_unprotect_via_stack
    s7_gc_protect_via_location
    s7_gc_unprotect_via_location
)
for i in "${s7_c_memory[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_memory.3 ${HOME}/man/man3/$i.3
done

s7_c_nulls=(
    s7_c_nulls

    s7_nil
    s7_undefined
    s7_unspecified
    s7_is_unspecified
    s7_eof_object
s7_is_null
)
for i in "${s7_c_nulls[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_nulls.3 ${HOME}/man/man3/$i.3
done

s7_c_numbers=(
    s7_c_numbers

    s7_is_number
    s7_is_integer
    s7_integer
    s7_make_integer
    s7_is_real
    s7_real
    s7_make_real
    s7_make_mutable_real
    s7_number_to_real
    s7_number_to_real_with_caller
    s7_number_to_integer
    s7_number_to_integer_with_caller
    s7_is_rational
    s7_is_ratio
    s7_make_ratio
    s7_rationalize
    s7_numerator
    s7_denominator
    s7_random
    s7_random_state
    s7_random_state_to_list
    s7_set_default_random_state
    s7_is_complex
    s7_make_complex
    s7_real_part
    s7_imag_part
    s7_number_to_string
    s7_big_real
    s7_big_integer
    s7_big_ratio
    s7_big_complex
    s7_is_bignum
    s7_is_big_real
    s7_is_big_integer
    s7_is_big_ratio
    s7_is_big_complex
    s7_make_big_real
    s7_make_big_integer
    s7_make_big_ratio
    s7_make_big_complex
)
for i in "${s7_c_numbers[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_numbers.3 ${HOME}/man/man3/$i.3
done

s7_c_pairs=(
    s7_c_pairs

    s7_is_pair
    s7_cons
    s7_car
    s7_cdr
    s7_set_car
    s7_set_cdr
    s7_cadr
    s7_cddr
    s7_cdar
    s7_caar
    s7_caadr
    s7_caddr
    s7_cadar
    s7_caaar
    s7_cdadr
    s7_cdddr
    s7_cddar
    s7_cdaar
    s7_caaadr
    s7_caaddr
    s7_caadar
    s7_caaaar
    s7_cadadr
    s7_cadddr
    s7_caddar
    s7_cadaar
    s7_cdaadr
    s7_cdaddr
    s7_cdadar
    s7_cdaaar
    s7_cddadr
    s7_cddddr
    s7_cdddar
    s7_cddaar
)
for i in "${s7_c_pairs[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_pairs.3 ${HOME}/man/man3/$i.3
done

s7_ports=(
    s7_ports
)
for i in "${s7_ports[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_ports.3 ${HOME}/man/man3/$i.3
done

s7_scm_equality=(
    s7_scm_equality

    s7_scm_eq
    s7_scm_eqv
    s7_scm_equal
    s7_scm_equivalent
)
for i in "${s7_scm_equality[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_scm_equality.3 ${HOME}/man/man3/$i.3
done

s7_scm_sequences=(
    s7_scm_sequences
)
for i in "${s7_scm_sequences[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_scm_sequences.3 ${HOME}/man/man3/$i.3
done

s7_c_strings=(
    s7_c_strings

    s7_is_string
    s7_string
    s7_make_string
    s7_make_string_with_length
    s7_make_string_wrapper
    s7_make_permanent_string
    s7_string_length
    s7_object_to_c_string
)
for i in "${s7_c_strings[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_strings.3 ${HOME}/man/man3/$i.3
done

s7_c_symbols=(
    s7_c_symbols

    s7_is_syntax
    s7_is_symbol
    s7_symbol_name
    s7_make_symbol
    s7_gensym
)
for i in "${s7_c_symbols[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_symbols.3 ${HOME}/man/man3/$i.3
done

s7_c_vectors=(
    s7_c_vectors

    s7_is_vector
    s7_vector_length
    s7_vector_rank
    s7_vector_dimension
    s7_vector_elements
    s7_int_vector_elements
    s7_float_vector_elements
    s7_is_float_vector
    s7_is_int_vector
    s7_vector_ref
    s7_vector_set
    s7_vector_ref_n
    s7_vector_set_n
    s7_vector_dimensions
    s7_vector_offsets
    s7_int_vector_ref
    s7_int_vector_set
    s7_float_vector_ref
    s7_float_vector_set
    s7_make_vector
    s7_make_int_vector
    s7_make_float_vector
    7_make_float_vector_wrapper
    s7_make_and_fill_vector
    s7_vector_fill
    s7_vector_copy
    s7_vector_to_list
)
for i in "${s7_c_vectors[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_c_vectors.3 ${HOME}/man/man3/$i.3
done

s7_readers=(
    s7_readers
)
for i in "${s7_readers[@]}"; do
    echo $i
    ln -s $MAN3DIR/s7_readers.3 ${HOME}/man/man3/$i.3
done

