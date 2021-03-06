<?php

/**
 * @file
 * Color.inc file.
 */

$info = array();

// Define the CSS file(s) that we want the Color module to use as a base.
$info['css'] = array('css/global.css');

// Define the possible replaceable items and their labels.
$info['fields'] = array(
  'color-body' => t('Body  color'),
  'color-white' => t('Boxes color'),
  'color-branding-secondary' => t('Secondary Branding color'),
  'color-branding-primary' => t('Primary Branding color'),
  'color-background-primary' => t('Primary background color'),
  'color-background-secondary' => t('Secondary background color'),
  'color-annotator-button' => t('Background for annotator button'),
);

// Color schemes for the site.
// Define the default scheme.
$info['schemes'] = array(
  // Scheme title.
  'default' => array(
    // Scheme colors.
    'title' => t('OS2dagsorden classic'),
    'colors' => array(
      'color-body' => '#fff',
      'color-white' => '#fffffe',
      'color-branding-secondary' => '#ae477d',
      'color-branding-primary' => '#9c1a5e',
      'color-background-primary' => '#f7f1f1',
      'color-background-secondary' => '#e3d6d6',
      'color-annotator-button' =>  '#e6e600'//'#3865f9'
    ),
    // Scheme title.
  ),
  'syddjurs_scheme' => array(
    // Scheme colors.
    'title' => t('OS2dagsorden lynglilla'),
    'colors' => array(
      'color-body' => '#fff',
      'color-white' => '#fff',
      'color-branding-secondary' => '#ae477d',
      'color-branding-primary' => '#9c1a5e',
      'color-background-primary' => '#f7f1f1',
      'color-background-secondary' => '#e3d6d6',
      'color-annotator-button' => '#e6e600'
    ),
    // Define the default scheme.
  ),
  // Scheme title.
  'scheme_blue' => array(
    // Scheme colors.
    'title' => t('OS2dagsorden blå'),
    'colors' => array(
      'color-body' => '#fff',
      'color-white' => '#fff',
      'color-branding-secondary' => '#2d8bc0',
      'color-branding-primary' => '#2c8abe',
      'color-background-primary' => '#fbfcfd',
      'color-background-secondary' => '#f2f4fb',
      'color-annotator-button' => '#e6e600'
    ),
  ),
);

// Files we want to copy along with the CSS files, let's define these later.
$info['copy'] = array();

// Files used in the scheme preview.
$info['preview_css'] = '';
$info['preview_js'] = '';
$info['preview_html'] = '';

// Gradients.
$info['gradients'] = array();

// Color areas to fill (x, y, width, height).
$info['fill'] = array('color-background-secondary' => array(0, 0, 101, 72));

// Coordinates of all the theme slices (x, y, width, height)
// With their filename as used in the stylesheet.
$info['slices'] = array('img/img_arrows_reverse.png' => array(0, 0, 101, 72));

// Base file for image generation.
$info['base_image'] = 'color/base.png';

$info['copy'] = array('logo.png');
