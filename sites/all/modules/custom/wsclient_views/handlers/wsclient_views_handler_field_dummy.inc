<?php

/**
 * @file
 * Base field handler that has no options and renders an unformatted field.
 */
class WSClientViewsHandlerFieldDummy extends views_handler_field {

  /**
   * Overriden to do nothing, really.
   * @see views_handler_field::query()
   */
  function query() {
    return;
  }

  /**
   * Render nothing
   * @see views_handler_field::render()
   */
  function render($values) {
    return '';
  }
}
