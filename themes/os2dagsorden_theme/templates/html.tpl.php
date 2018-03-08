<?php

/**
 * @file
 * Html.tpl.php file.
 */

print $doctype; ?>
<html lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>"<?php print $rdf->version . $rdf->namespaces; ?>>
<head<?php print $rdf->profile; ?>>
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <link rel="apple-touch-icon" href="/sites/default/files/icons/apple-touch-icon.png" />
  <link rel="apple-touch-icon" sizes="57x57" href="/sites/default/files/icons/apple-touch-icon-57x57.png" />
  <link rel="apple-touch-icon" sizes="72x72" href="/sites/default/files/icons/apple-touch-icon-72x72.png" />
  <link rel="apple-touch-icon" sizes="76x76" href="/sites/default/files/icons/apple-touch-icon-76x76.png" />
  <link rel="apple-touch-icon" sizes="114x114" href="/sites/default/files/icons/apple-touch-icon-114x114.png" />
  <link rel="apple-touch-icon" sizes="120x120" href="/sites/default/files/icons/apple-touch-icon-120x120.png" />
  <link rel="apple-touch-icon" sizes="144x144" href="/sites/default/files/icons/apple-touch-icon-144x144.png" />
  <link rel="apple-touch-icon" sizes="152x152" href="/sites/default/files/icons/apple-touch-icon-152x152.png" />
  <link rel="apple-touch-icon" sizes="180x180" href="/sites/default/files/icons/apple-touch-icon-180x180.png" />
  <?php print $styles; ?>
  <?php print $scripts; ?>
  <!--[if lt IE 9]><script src="//html5shiv.googlecode.com/svn/trunk/html5.js"></script><![endif]-->
</head>
<body<?php print $attributes;?>>
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>
  <?php print $page_top; ?>
  <?php print $page; ?>
  <?php print $page_bottom; ?>
</body>
</html>
