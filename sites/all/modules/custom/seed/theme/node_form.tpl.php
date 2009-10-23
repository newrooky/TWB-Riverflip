<div class='form clear-block <?php print $form_classes ?>'>

<?php if ($sidebar): ?>
  <div class='sidebar'><?php print drupal_render($sidebar) ?></div>
<?php endif; ?>

<div class='main'>
  <?php
    $form_settings = variable_get('seed_forms', array());
    print !empty($form_settings['numbered']) ? seed_number_form($form) : drupal_render($form)
  ?>
</div>

<?php if ($buttons): ?>
  <div class='buttons clear-block'><?php print drupal_render($buttons) ?></div>
<?php endif; ?>

<?php if ($hidden): ?>
  <div class='hidden' style='display:none'><?php print drupal_render($hidden) ?></div>
<?php endif; ?>

</div>
