<?php
  $current_theme = variable_get('theme_default', 'none');
  $logo = theme_get_setting('logo', $current_theme);
?>

<header<?php print $attributes; ?>>
  <?php print $content; ?>
</header>

<!-- Begin - responsive header -->
<div class="responsive-header">
  <div class="flexy-row">
    <a href="/">
      <img src="<?=$logo; ?>" alt="OS2dagsorden"/>
    </a>

    <div class="flexy-spacer"></div>

    <div>
      <a href="/meetings-search" class="responsive-button responsive-button--search">
        <i class="material-icons">search</i>
      </a>

      <button class="responsive-button responsive-button--sidr-toggle">
        <i class="material-icons">menu</i>
      </button>
    </div>
  </div>
</div>
<!-- End - responsive header -->
