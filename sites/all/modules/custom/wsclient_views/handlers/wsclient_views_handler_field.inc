<?php

/**
 * @file
 * Base field handler that has no options and renders an unformatted field.
 *
 * Definition terms:
 * - additional fields: An array of fields that should be added to the query
 *                      for some purpose. The array is in the form of:
 *                      array('identifier' => array('table' => tablename,
 *                      'field' => fieldname); as many fields as are necessary
 *                      may be in this array.
 * - click sortable: If TRUE, this field may be click sorted.
 */
class WSClientViewsHandlerField extends views_handler_field {

/**
 * Called to add the field to a query.
 */
function query() {
  // Add the field.
  $this->field_alias = $this->real_field;
}

 /**
  * Render the field.
  *
  * @param $values
  *   The values retrieved from the database.
  */
 function render($values) {
   $value = $values->{$this->field_alias};
   return check_plain($value);
 }
}
