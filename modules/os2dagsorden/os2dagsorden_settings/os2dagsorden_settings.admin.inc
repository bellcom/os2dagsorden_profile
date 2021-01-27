<?php

/**
 * @file
 * Os2dagsorden_settings.
 *
 * PHP version 5
 *
 * @category OS2dagsorden
 * @package OS2dagsorden_Settings
 *  Admin page file
 * @author Stanislav Kutasevits <stan@bellcom.dk>
 * @license http://www.gnu.org/licenses/gpl-2.0.html GNU General Public License, version 2
 * @link http://bellcom.dk
 */

define('ACADRE_MM_IMPORT_DIR', 'private://');
define('OS2DAGSORDEN_HIDDEN_AGENGA_COMMITTEE', '28');

/**
 * Implementation of settings form specifically for OS2dagsorden system.
 *
 * @param mixed $form
 *   The settings form.
 * @param mixed $form_state
 *   The state of the form.
 *
 * @return mixed
 *   Settings form.
 */
function os2dagsorden_settings_settings_form($form, $form_state) {
  drupal_add_js(drupal_get_path('module', 'os2dagsorden_settings') . '/js/os2dagsorden_settings.js');

  $output = os2dagsorden_blocks_positioning_show_git_version();
  $info_path = drupal_get_path('profile', 'os2dagsorden') . '/os2dagsorden.info';
  $info = drupal_parse_info_file($info_path);
  $version = (isset($info['version'])) ? $info['version'] : 'no version';

  $form['os2dagsorden_version_text'] = array(
    '#type' => 'item',
    '#weight' => -10,
    '#title' => t('OS2dagsorden - versionsnummer'),
    '#markup' => '<div class= "text">' . $output . '</div>',
  );
  variable_set('os2dagsorden_version_no', variable_get('os2dagsorden_version_no', 1.0));
  $link = '';
  if ($version > variable_get('os2dagsorden_version_no')) {
    $link = '<td> <a href="/admin/updatedb">' . t('Gå her til at opdatere DB') . '</a></td>';
  }
  $form['os2dagsorden_version_no'] = array(
    '#type' => 'item',
    '#weight' => -9,
    '#title' => t('OS2dagsorden - opdatering version'),
    '#markup' => '<div class="text"><table><tr><td style = "width:17%">' . t('DB opdatering version:') . ' </td><td> <b> ' . variable_get('os2dagsorden_version_no') . '</b></td>' . $link . '</tr></table></div>',
  );
  $form['os2dagsorden_settings_file_fieldset'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Settings file'),
  );

  $form['os2dagsorden_settings_file_fieldset'][] = array(
    '#type' => 'link',
    '#preffix' => '<p>',
    '#href' => 'os2dagsorden/export_settings',
    '#title' => t('Download current settings as a file'),
    '#suffix' => '</p>',
  );

  $settings_file = NULL;
  if (variable_get('os2dagsorden_settings_file', FALSE)) {
    $settings_file = file_load(variable_get('os2dagsorden_settings_file'));
  }

  if ($settings_file) {
    $form['os2dagsorden_settings_file_fieldset'][] = array(
      '#markup' => '<p>' . t('File in use:') . ' ' . (($settings_file) ? $settings_file->filename : 'none') . '</p>',
    );
  }

  $form['os2dagsorden_settings_file_fieldset']['os2dagsorden_settings_file'] = array(
    '#title' => t('New settings file'),
    '#type' => 'managed_file',
    '#description' => t('Extract the settings from the file'),
    '#upload_location' => 'public://os2dagsorden_settings',
    '#upload_validators' => array(
      'file_validate_extensions' => array('settings'),
    ),
  );

  $form['os2dagsorden_settings_file_fieldset']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Import settings from file'),
    '#submit' => array('os2dagsorden_settings_import_from_file'),
  );

  $form['os2dagsorden_meeting_view_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Meeting view settings'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_unpublish_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show "Unpublish meeting" button on agenda page'),
    '#default_value' => variable_get('os2dagsorden_show_meeting_unpublish_button', TRUE),
    '#description' => t('This decides whether a buttom for unpublishing meeting will be shown on  meeting view.'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_import_form_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show "Import form" button on agenda page'),
    '#default_value' => variable_get('os2dagsorden_show_import_form_button', TRUE),
    '#description' => t('This decides whether a button for import meetings will be shown on  meeting view.'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_committee'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting committee on meeting view'),
    '#description' => t('This decides whether a meeting committee is visible on meeting view.'),
    '#default_value' => variable_get('os2dagsorden_show_meeting_committee', TRUE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_enddate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting end date on meeting view'),
    '#description' => t('This decides whether a meeting end date is visible on meeting view.'),
    '#default_value' => variable_get('os2dagsorden_show_meeting_enddate', TRUE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_type'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting type on meeting view'),
    '#description' => t('This decides whether a meeting type field is visible on meeting view.'),
    '#default_value' => variable_get('os2dagsorden_show_meeting_type', TRUE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_view_settings_not_attach_addtional_agenda'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('os2dagsorden_meeting_view_settings_not_attach_addtional_agenda', FALSE),
    '#title' => t("Don't attach tillægsdagsordener to meeting"),
    '#description' => ('This decides whether a additional agenda will be attached to the meeting')
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_user_no_roles_permissions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Users with "no roles" have same permissions as "os2dagsorden limited" role'),
    '#default_value' => variable_get('os2dagsorden_show_user_no_roles_permissions', FALSE),
    '#description' => t('This decides whether a users that have no roles will be have same access to meetings as users with "os2dagsorden limited" role.'),
  );
  if (module_exists('os2web_print_send_to_friend')) {
    $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_print_icon'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show print icon on bullets in agenda'),
      '#description' => t('This decides whether a print button is visible on each bullet point on meeting view.'),
      '#default_value' => variable_get('os2dagsorden_show_print_icon', FALSE),
    );
    $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_send_to_friend_icon'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show send to friend icon on bullets in agenda'),
      '#description' => t('This decides whether a send to friend button is visible on each bullet point on meeting view.'),
      '#default_value' => variable_get('os2dagsorden_show_send_to_friend_icon', FALSE),
    );
    $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_download_description_button_on_bp_view'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show download descripttion button on bullet point view'),
      '#description' => t('This decides whether a download descripttion button is visible on bullet point view.'),
      '#default_value' => variable_get('os2dagsorden_show_download_description_button_on_bp_view', FALSE),
    );
    $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_generate_pdf_link'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show generate PDF link on meeting view'),
      '#description' => t('This decides whether a generate PDF link is visible on meeting view.'),
      '#default_value' => variable_get('os2dagsorden_show_generate_pdf_link', TRUE),
    );
    $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_generate_pdf_link_full'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show generate PDF link (full) on meeting view'),
      '#description' => t('This decides whether a generate PDF link (with attachments) is visible on meeting view.'),
      '#default_value' => variable_get('os2dagsorden_show_generate_pdf_link_full', TRUE),
    );
  }
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_participants_afbud'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show cancelled participants'),
    '#description' => t('This decides whether a list of cancelled participants  is visible on meeting view'),
    '#default_value' => variable_get('os2dagsorden_show_participants_afbud', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bullet_case_id_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show case id link'),
    '#description' => t('This decides whether case id is visible on each bullet point on the meeting view (assuming bullet point has case id provided)'),
    '#default_value' => variable_get('os2dagsorden_show_bullet_case_id_link', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_bullet_case_id_link_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Case nr link'),
    '#description' => t('That is the case cr link prefix that is linking to external case presentation system. Case identifier will added to the end of the provided link'),
    '#default_value' => variable_get('os2dagsorden_bullet_case_id_link_url', ""),
    '#states' => array(
      'visible' => array(
        ':input[name="os2dagsorden_show_bullet_case_id_link"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_comments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show comments on meeting view'),
    '#description' => t('This decides whether a comments field is visible on meeting view.'),
    '#default_value' => variable_get('os2dagsorden_show_comments', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_closed_bullet_point_content'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show closed bullet point content on directly on meeting view'),
    '#description' => t('This decides whether a closed bullet point content will shown the same way as open bullet point - directly on the meeting view.'),
    '#default_value' => variable_get('os2dagsorden_show_closed_bullet_point_content', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_open_closed_bullet_in_new_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open closed bullet points in new tab'),
    '#description' => t('This decides whether a closed bullet point will open in new browser window.'),
    '#default_value' => variable_get('os2dagsorden_open_closed_bullet_in_new_tab', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_closed_bullet_content_depending_of_publishlevel'] = array(
    '#type' => 'checkbox',
    '#title' => t('show closed bullet points dependging of publishlevel'),
    '#description' => t('This decides whether a closed bullet point content will be shown based on publishlelel settings.'),
    '#default_value' => variable_get('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bilag'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show bullet point attachment (bilag) on meeting view and bullet point attachment views'),
    '#description' => t('This decides whether a bullet point attachment converted from PDF to HTML is going to be shown in the application.<br/>Can be useful to disable display if users are known to used old browser versions (>IE8).'),
    '#default_value' => variable_get('os2dagsorden_show_bilag', TRUE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_use_abbr'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use abbreviation of the meeting in the meeting views'),
    '#description' => t('This decides whether in some places a short version (abbreviation) of the meeting will be used instead of the full title'),
    '#default_value' => variable_get('os2dagsorden_use_abbr', TRUE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bp_title_system_text'] = array(
    '#type' => 'checkbox',
    '#title' => t('Vis systemtekst <strong>"Punkt x"</strong> i punkttitel' ),
    '#description' => t('Dette beslutter, om systemteksten <strong>"Punkt x"</strong> skal vises i punkttitel'),
    '#default_value' => variable_get('os2dagsorden_show_bp_title_system_text', TRUE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bullet_case_id_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show case id link'),
    '#description' => t('This decides whether case id is visible on each bullet point on the meeting view (assuming bullet point has case id provided)'),
    '#default_value' => variable_get('os2dagsorden_show_bullet_case_id_link', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bilag_pages_amount'] = array(
    '#type' => 'checkbox',
    '#title' => t('Vis antal sider i bilag'),
    '#default_value' => variable_get('os2dagsorden_show_bilag_pages_amount', FALSE),
    '#description' => t('Dette afgør, om mængden af ​​BPA-sider vises på mødesiden'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_show_group_pager_on_bpa_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Vis sidegruppering på vedhæftede side med punkt'),
    '#default_value' => variable_get('os2dagsorden_show_group_pager_on_bpa_page', FALSE),
    '#description' => t('Dette beslutter, om "grupper" af sider vil blive vist i bunden af ​​BPA-apgee'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_pdf_download_whitelist_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Aktivér whitelisting til PDF-download'),
    '#description' => t("Dette beslutter, om PDF-download til møde og vedhæftede filer kun er tilladt til whitelistede IP'er"),
    '#default_value' => variable_get('os2dagsorden_pdf_download_whitelist_enabled', FALSE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_search_restrict_search_function'] = array(
    '#type' => 'checkbox',
    '#title' => t('Begræns søgefunktion, når du er på en mødeside'),
    '#default_value' => variable_get('os2dagsorden_search_restrict_search_function', FALSE),
    '#description' => t('Dette afgør, om fritekstsøgningen på mødesiden kun søges i det møde, du er på'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_pdf_download_whitelist'] = array(
    '#type' => 'textfield',
    '#title' => t('PDF download whitelist'),
    '#description' => t("Provide list of IP's, to which PDF download is visible. Partial matches are also possible<br>E.g. 192.168.1.1, or 192.168.1"),
    '#default_value' => variable_get('os2dagsorden_pdf_download_whitelist', FALSE),
    '#states' => array(
      'visible' => array(
        ':input[name="os2dagsorden_pdf_download_whitelist_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links'] = array(
    '#type' => 'checkbox',
    '#title' => t('Begge links til åbne og lukkede dagsordener er aktiveret til download'),
    '#default_value' => variable_get('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', FALSE),
    '#description' => t('Dette afgør, om der på mødesiden vises links til download af både åbne og lukkede dagsordener, hvis bruger har tilladelse til at se dem, eller kun link til kun downloding-dagsorden med den aktuelle status'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_sager_group_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Sager group title'),
    '#description' => t('This decides how a group of Cases/Sager will be titled'),
    '#default_value' => variable_get('os2dagsorden_sager_group_title', 'Sager'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_group_closed_bullet_points'] = array(
    '#type' => 'radios',
    '#title' => t('Group closed closed bullet points as a separate block'),
    '#default_value' => variable_get('os2dagsorden_group_closed_bullet_points', 'false'),
    '#options' => array('true' => t('Group separately'), 'false' => t('Put inline')),
    '#description' => t('This decides whether closed bullet points will be grouped separately as a additional block called "Lukket dagsorden", or put inline together the rest of the bullet points.'),
  );
  // Hidden agenda.
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_hidden_agenda_committee'] = array(
    '#type' => 'textfield',
    '#title' => t('List of committees which agenda is completely hidden to those without the rights'),
    '#maxlength' => 200,
    '#description' => t('Comma-separated list of committees ID which agenda and the summary (samlet dokument) is hidden to those without the rights'),
    '#default_value' => variable_get('os2dagsorden_hidden_agenda_committee', OS2DAGSORDEN_HIDDEN_AGENGA_COMMITTEE),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_users_accessed_hidden_agenda_committee'] = array(
    '#type' => 'textfield',
    '#title' => t('Brugere, der kan se skjulte dagsordener'),
    '#size' => 40,
    '#maxlength' => 60,
    '#autocomplete_path' => 'get_users/autocomplete',
    '#description' => t('Kommasepareret liste over brugere, der kan se skjulte dagsordener'),
    '#default_value' => variable_get('os2dagsorden_users_accessed_hidden_agenda_committee', ''),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_download_dropdown'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting description download dropdown button'),
    '#maxlength' => 200,
    '#description' => t('Title of the meeting description download dropdown button'),
    '#default_value' => variable_get('os2dagsorden_meeting_description_download_dropdown', 'Hent dokumenter'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_generate_pdf_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Mødebeskrivelse genererer PDF-knap'),
    '#maxlength' => 200,
    '#description' => t('Titel på mødebeskrivelsen PDF-genereringsknap'),
    '#default_value' => variable_get('os2dagsorden_meeting_description_generate_pdf_title', 'Samlet dokument i PDF'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_generate_pdf_full_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Mødebeskrivelse genererer fuld PDF-knap'),
    '#maxlength' => 200,
    '#description' => t('Titel på mødebeskrivelsen fuld PDF-genereringsknap'),
    '#default_value' => variable_get('os2dagsorden_meeting_description_generate_pdf_full_title', 'Samlet dokument med bilag i PDF'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_with_att_download_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting description with attachments download button'),
    '#maxlength' => 200,
    '#description' => t('Title of the meeting description with attachments download button'),
    '#default_value' => variable_get('os2dagsorden_meeting_description_with_att_download_title', 'Samlet dokument med bilag'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_download_title_additional'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting additional description download button'),
    '#maxlength' => 200,
    '#description' => t('Title of the meeting additional description download button'),
    '#default_value' => variable_get('os2dagsorden_meeting_description_download_title_additional', 'Hent tillægsdokument i PDF'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_with_att_download_title_additional'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting additional description with attachments download button'),
    '#maxlength' => 200,
    '#description' => t('Title of the meeting additional description with attachments download button'),
    '#default_value' => variable_get('os2dagsorden_meeting_description_with_att_download_title_additional', 'Tillægsdokument med bilag'),
  );
  $form['os2dagsorden_meeting_view_settings']['print_pdf_page_orientation'] = array(
    '#type' => 'select',
    '#title' => t('Page orientation in PDF file'),
    '#options' => array('portrait' => t('Portrait'), 'landscape' => t('Landscape')),
    '#default_value' => variable_get('print_pdf_page_orientation', PRINT_PDF_PAGE_ORIENTATION_DEFAULT),
    '#description' => t('Choose the page orientation of the generated PDF.'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_print_pdf_show_commitee'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show committee and meeting date in bullet point title'),
    '#default_value' => variable_get('os2dagsorden_print_pdf_show_commitee', FALSE),
    '#description' => t('This decides whether meetimg committee and date will be shown in bullet point title in PDF'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_create_note_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Opret note knappetekst'),
    '#maxlength' => 200,
    '#description' => t('Tekst, der vises på knappen Opret note'),
    '#default_value' => variable_get('os2dagsorden_create_note_text', 'Lav note'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_search_switch_to_note_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Skift til tekstknaptekst'),
    '#maxlength' => 200,
    '#description' => t('Tekst, der vises på knappen Skift til note'),
    '#default_value' => variable_get('os2dagsorden_search_switch_to_note_text', 'Skift til noter'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_create_note_hide_text'] = array(
    '#type' => 'checkbox',
    '#title' => t('Opret note knap skjule tekst'),
    '#default_value' => variable_get('os2dagsorden_create_note_hide_text', FALSE),
    '#description' => t('Tekst på knappen Opret note bliver skjult'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_create_note_text_use_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Opret note knap, sæt tekst til titelattribut'),
    '#default_value' => variable_get('os2dagsorden_create_note_text_use_title', FALSE),
    '#description' => t('Teksten sættes også i titlen HTML-attribut for en knap (tilgængelig på musemarkøren)'),
  );
  $form['os2dagsorden_meeting_view_settings']['os2dagsorden_notes_page_standart_layout'] = array(
    '#type' => 'checkbox',
    '#title' => t('Brug standard notelayout'),
    '#default_value' => variable_get('os2dagsorden_notes_page_standart_layout', TRUE),
    '#description' => t('Dette bestemmer, hvilket layout der skal bruges til notesiden'),
  );

  // Expand or not expand bilag on bullet point expand
  // Meetig view settings.
  $form['os2dagsorden_meeting_view_layout'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Meeting view layout'),
  );

  // Meeting view settings.
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_hide_meeting_details'] = array(
    '#type' => 'checkbox',
    '#title' => t('Hide meeting details on meeting layout'),
    '#description' => t('Check this box to hide the meeting details in the top of the meeting presentation'),
    '#default_value' => variable_get('os2dagsorden_hide_meeting_details', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_all_bullets'] = array(
    '#type' => 'checkbox',
    '#title' => t('Expand all bullets on agenda page load'),
    '#description' => t('This decides whether all bullet points will be expanded on page load'),
    '#default_value' => variable_get('os2dagsorden_expand_all_bullets', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_show_expand_all_button'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show "Expand all" button meeting page'),
    '#description' => t('This decides "Expand all" button will be shown next to the bullet title'),
    '#default_value' => variable_get('os2dagsorden_show_expand_all_button', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_attachment_onload'] = array(
    '#type' => 'checkbox',
    '#title' => t('Expand all attachments on bullet point open'),
    '#description' => t('This decides whether all attachments will be expanded on bullet point open'),
    '#default_value' => variable_get('os2dagsorden_expand_attachment_onload', FALSE),
  );
  $options = array(
    'true' => t('Expand'),
    'false' => t('Not expand (but keep it that way if the bilag has been expanded before)'),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_bilags'] = array(
    '#type' => 'radios',
    '#title' => t('Expand Bilag'),
    '#default_value' => variable_get('os2dagsorden_expand_bilags', 'true'),
    '#options' => $options,
    '#description' => t('This decides whether a certain type of bullet point attachment - bilag will be expanded (activated) when functionality "Expand whole bullet point" is executed.'),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_cases'] = array(
    '#type' => 'radios',
    '#title' => t('Expand Sag'),
    '#default_value' => variable_get('os2dagsorden_expand_cases', 'false'),
    '#options' => $options,
    '#description' => t('This decides whether a certain type of bullet point attachment -case will be expanded (activated) when functionality "Expand whole bullet point" is executed.'),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_show_massive_expand_collapse_button'] = array(
    '#type' => 'radios',
    '#title' => t('Massive expand/collapse button'),
    '#default_value' => variable_get('os2dagsorden_show_massive_expand_collapse_button', 'true'),
    '#options' => array('true' => t('Show'), 'false' => t('Hide')),
    '#description' => t('This decides whether massive expand/collapse button will be shown on meeting views.'),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_show_all_bilags_list_on_attachment_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Vis alle punktets bilag som liste på visningssiden.'),
    '#description' => t('Dette afgør, om alle punktets bilag vises i bilagsvisningen'),
    '#default_value' => variable_get('os2dagsorden_show_all_bilags_list_on_attachment_page', FALSE),
  );
   $form['os2dagsorden_meeting_view_layout']['os2dagsorden_show_bilag_bullet_point'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show bilags under Bilag bullet point'),
    '#description' => t('This decides whether all bilags grouped under Bilag bullet point'),
    '#default_value' => variable_get('os2dagsorden_show_bilag_bullet_point', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_show_case_bullet_point'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show cases under Case bullet point'),
    '#description' => t('This decides whether all Cases/Sager grouped under Cases bullet point'),
    '#default_value' => variable_get('os2dagsorden_show_case_bullet_point', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_show_bullet_case_nr'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show case nr next to bullet title'),
    '#description' => t('This decides case nr will be shown next to the bullet title'),
    '#default_value' => variable_get('os2dagsorden_show_bullet_case_nr', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_desktop'] = array(
    '#type' => 'textfield',
    '#title' => t('Hvor mange sider vises i bilag på møder på stationære enheder'),
    '#description' => t('Dette afgør, hvor mange sider der skal vises i bilag filer på skrivebordsenheder..'),
    '#maxlength' => 10,
    '#size' => 10,
    '#field_suffix' => t('Pages'),
    '#default_value' => variable_get('os2dagsorden_attachments_pages_per_view_desktop', 5),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_desktop_meeting_veiw'] = array(
    '#type' => 'checkbox',
    '#title' => t('Aktivér også til mødevisning'),
    '#default_value' => variable_get('os2dagsorden_attachments_pages_per_view_desktop_meeting_veiw', FALSE),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Hvor mange sider vises i bilag på møder på mobile enheder'),
    '#description' => t('Dette afgør, hvor mange sider der skal vises i vedhæftede filer på mobile enheder.'),
    '#maxlength' => 10,
    '#size' => 10,
    '#field_suffix' => t('Pages'),
    '#default_value' => variable_get('os2dagsorden_attachments_pages_per_view_mobile', 3),
  );
   $form['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_mobile_meeting_veiw'] = array(
    '#type' => 'checkbox',
    '#title' => t('Aktivér også til mødevisning'),
    '#default_value' => variable_get('os2dagsorden_attachments_pages_per_view_mobile_meeting_veiw', FALSE),
  );
   $form['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_show_scrollbar_on_attachment_page'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Vis rullepanel på bilag'),
    '#options' => array( 'thumbnails' => t('På thumbnail pagineringsområde'), 'attachment' => t('På indholdsområdet i bilag')),
    '#description' => t('Dette beslutter, om rullebjælken skal vises, når bilag vises som HTML.'),
    '#default_value' => variable_get('os2dagsorden_attachments_show_scrollbar_on_attachment_page', ''),
  );
  $form['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_open_bilags_as_new_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Åbn bilag i ny fane'),
    '#description' => t('Dette bestemmer, om bilag, der åbnes fra vedhæftningsmenuen, vil åbne som en ny fane.'),
    '#default_value' => variable_get('os2dagsorden_attachments_open_bilags_as_new_tab', FALSE),
  );

  // Show latest meetings in my committee or all committees.
  $form['os2dagsorden_latest_meetings_view_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Latest 8 meetings/upcoming meetings'),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_latest_meetings_view_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Switch beetween all/my committees meetings'),
    '#options' => array(
      'all' => t('Show meetings from all committees'),
      'mine' => t('Show meetings from my committees only'),
    ),
    '#default_value' => variable_get('os2dagsorden_latest_meetings_view_mode', 'all'),
    '#description' => t('This decides whether the front page view will show 8 latest meeting from all committees or only from user committees (member of, follows, follows+)'),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting title on latest 8 meetings/upcoming meetings view'),
    '#description' => t('This decides whether a meeting title is visible on latest 8 meetings/upcoming meetings view.'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_show_meeting_title', TRUE),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_committee'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting committee on latest 8 meetings/upcoming meetings view'),
    '#description' => t('This decides whether a meeting committee is visible on latest 8 meetings/upcoming meetings view.'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_show_meeting_committee', TRUE),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_location'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting location on latest 8 meetings/upcoming meetings view'),
    '#description' => t('This decides whether a meeting location is visible on latest 8 meetings/upcoming meetings view.'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_show_meeting_location', TRUE),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_is_closed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show closed indicator on latest 8 meetings/upcoming meetings view'),
    '#description' => t('This decides whether a closed indicator will be shown next to meeting if meeting has at least one bullet that is closed'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_show_meeting_is_closed', FALSE),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_meeting_title_len'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting title lenght'),
    '#description' => t('This specify how many characters the "møde column" should crop the text at. If this is set to 0 then we show the full title.'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_meeting_title_len', 25),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_meeting_location_len'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting location lenght'),
    '#description' => t('This specify how many characters the location column should crop the text at. If this is set to 0 then we show the full title.'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_meeting_location_len', 25),
  );
  $form['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_external_committee'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only members of external committee can see meetings in external committee'),
    '#description' => t('This decides whether a meeting from external committee is visible for users that are not committe members.'),
    '#default_value' => variable_get('os2dagsorden_meetings_views_last_8_show_external_committee', TRUE),
  );

  // Frontpage layout settings.
  $form['os2dagsorden_frontpage_layout'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Frontpage layout'),
  );
  $options3 = array('false' => t('Expand'), 'true' => t('Collapse'));
  $form['os2dagsorden_frontpage_layout']['os2dagsorden_collapse_menu'] = array(
    '#type' => 'radios',
    '#title' => t('Collapse/expand menu'),
    '#default_value' => variable_get('os2dagsorden_collapse_menu', TRUE),
    '#options' => $options3,
    '#description' => t('This decides whether a side menu expanded/collapsed.'),
  );
  $form['os2dagsorden_frontpage_layout']['os2dagsorden_collapse_menu_touch'] = array(
    '#type' => 'radios',
    '#title' => t('Collapse/expand menu touch devices'),
    '#default_value' => variable_get('os2dagsorden_collapse_menu_touch', TRUE),
    '#options' => $options3,
    '#description' => t('This decides whether a side menu expanded/collapsed for touch devices (tabels, phone etc).'),
  );
  $form['os2dagsorden_frontpage_layout']['os2dagsorden_collapse_menu_touch_pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Hide side menu on those pages (touch)'),
    '#default_value' => variable_get('os2dagsorden_collapse_menu_touch_pages', 'meetings/print'),
    '#description' => t('List the pages where the side menu will be hidden by default (overruling the settings).<br/>Each page on new line'),
  );

  $form['os2dagsorden_frontpage_layout']['os2dagsorden_show_search_block_title'] = array(
    '#type' => 'radios',
    '#title' => t('Show/Hide search block title'),
    '#default_value' => variable_get('os2dagsorden_show_search_block_title', 'true'),
    '#options' => array('true' => t('Show'), 'false' => t('Hide')),
    '#description' => t('This decides whether a search block title show/hide.'),
  );
  $form['os2dagsorden_frontpage_layout']['os2dagsorden_frontpage_layout_show_hide_zoom_buttons'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show/Hide zoom buttons'),
    '#description' => t('This decides whether the zoom buttons will be shown'),
    '#default_value' => variable_get('os2dagsorden_frontpage_layout_show_hide_zoom_buttons', FALSE),
  );

  // Body text size.
  $form['os2dagsorden_text_layout'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Text sizes'),
  );
  $form['os2dagsorden_text_layout']['os2dagsorden_body_text_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Body text size'),
    '#maxlength' => 10,
    '#size' => 10,
    '#field_suffix' => ' px',
    '#default_value' => variable_get('os2dagsorden_body_text_size', '13'),
    '#description' => t('Default body text font size is 13 px. When you change the body text size, then please clear the cache to apply change'),
  );

  // Title text size.
  $form['os2dagsorden_text_layout']['os2dagsorden_title_text_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Title text size'),
    '#maxlength' => 10,
    '#size' => 10,
    '#field_suffix' => ' px',
    '#default_value' => variable_get('os2dagsorden_title_text_size', '13'),
    '#description' => t('Default title text font size is 13 px. When you change the title text size, then please clear the cache to apply change'),
  );

  // Import meeting in unpublished state.
  $form['os2dagsorden_meeting_unpublished_state'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Meeting in unpublished state notification'),
  );
  $form['os2dagsorden_meeting_unpublished_state']['os2dagsorden_meeting_unpublished_state_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('Add subject'),
    '#default_value' => variable_get('os2dagsorden_meeting_unpublished_state_subject', ''),
  );
  $form['os2dagsorden_meeting_unpublished_state']['os2dagsorden_meeting_unpublished_state_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#description' => t('Add text'),
    '#default_value' => variable_get('os2dagsorden_meeting_unpublished_state_body', ''),
  );
  $form['os2dagsorden_meeting_unpublished_state']['os2dagsorden_meeting_unpublished_state_email_text_available_variables'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => t('Can be used both in subject and body:') . '<br/>
    <b>!committe</b> - ' . t('name of the committee') . '<br/>
    <b>!meeting_name</b> - ' . t('title of the meeting') . '<br/>
    <b>!meeting_date</b> - ' . t('date of the meeting') . '<br/>
    <b>!meeting_type</b> - ' . t('type of the meeting with acticle: <i>et referat</i> or <i>en dagsorden</i>') . '<br/>
    <b>!meeting_url</b> - ' . t('link to the meeting'),
    '#title' => t('Available variables'),
  );

  // Meeting search view settings.
  $form['os2dagsorden_meeting_search_view_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Meeting search view settings'),
  );
  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_show_meeting_committee'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting committee on meeting search view'),
    '#description' => t('This decides whether a meeting committee is visible on meeting search view.'),
    '#default_value' => variable_get('os2dagsorden_meetings_search_show_meeting_committee', TRUE),
  );
  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_show_meeting_location'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting location on meeting search view'),
    '#description' => t('This decides whether a meeting location is visible on meeting search view.'),
    '#default_value' => variable_get('os2dagsorden_meetings_search_show_meeting_location', FALSE),
  );
  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_show_meeting_status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show meeting status on meeting search view'),
    '#description' => t('This decides whether a meeting status is visible on meeting search view.'),
    '#default_value' => variable_get('os2dagsorden_meetings_search_show_meeting_status', FALSE),
  );
  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_include_add_agenda'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include additional agendas into search by committee (empty keyword)'),
    '#description' => t('This decides whether the additional agenda meeting (tillaæegsdagsorden) will be included into the search results, when keyword is empty.<br/>
      If keyword is not empty additional agenda meeting will be included disregarding of this setting.'),
    '#default_value' => variable_get('os2dagsorden_meetings_search_include_add_agenda', TRUE),
  );

  $month_choise = array();
  foreach (range(0, 12) as $number) {
    $month_choise[$number] = $number;
  }

  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_search_startdate'] = array(
    '#type' => 'select',
    '#title' => t('Search start date'),
    '#field_suffix' => t('months before today'),
    '#description' => t('This sets the default search start date for meeting search'),
    '#options' => $month_choise,
    '#default_value' => variable_get('os2dagsorden_search_startdate', 1),
  );

  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_search_enddate'] = array(
    '#type' => 'select',
    '#title' => t('Search end date'),
    '#field_suffix' => t('months after today'),
    '#description' => t('This sets the default search end date for meeting search'),
    '#options' => $month_choise,
    '#default_value' => variable_get('os2dagsorden_search_enddate', 1),
  );
  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_search_meeting_is_closed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show closed indicator on meetings search view'),
    '#description' => t('This decides whether a closed indicator will be shown next to meeting if meeting has at least one bullet that is closed'),
    '#default_value' => variable_get('os2dagsorden_search_meeting_is_closed', FALSE),
  );
  $form['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_overview_remember_filter_period'] = array(
    '#type' => 'textfield',
    '#title' => t('Meetings overview filter remember period'),
    '#field_suffix' => t('minutes'),
    '#size' => 5,
    '#default_value' => variable_get('os2dagsorden_meetings_overview_remember_filter_period', 60),
    '#description' => t('Number of minutes to remember meetings overview exposed form values.'),
  );

  // Sidemenu layout settings.
  $form['os2dagsorden_sidepane_menu_layout'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Sidepane menu layout'),
  );

  $form['os2dagsorden_sidepane_menu_layout']['main_menu_links'] = array(
    '#type' => 'link',
    '#prefix' => '<p>',
    '#href' => 'admin/structure/menu/manage/menu-sidepane-menu',
    '#title' => t('Change order of the main menu links'),
    '#sufix' => '</p>',
  );
  $form['os2dagsorden_sidepane_menu_layout']['sipepane_blocks_order'] = array(
    '#type' => 'link',
    '#prefix' => '<p>',
    '#href' => 'admin/structure/block',
    '#title' => t('Change order of sidepane blocks (Sidebar Second)'),
    '#sufix' => '</p>',
  );
  $vid = db_select('taxonomy_vocabulary', 'tv')
    ->fields('tv', array('vid'))
    ->condition('machine_name', 'os2web_meetings_tax_committee')
    ->execute()
    ->fetchField();

  $committees = taxonomy_get_tree($vid);
  $options_committee = array();
  foreach ($committees as $committee) {
    $options_committee[$committee->tid] = $committee->name;
  }

  $form['os2dagsorden_sidepane_menu_layout']['os2dagsorden_budget_committee'] = array(
    '#type' => 'select',
    '#title' => t('Select a committee which meeting will be available in <b>Budget</b> menu link'),
    '#description' => t('This decides committee will update the meeting link in Budget menu link in the side panel'),
    '#options' => $options_committee,
    '#empty_option' => '- ' . t('None') . ' -',
    '#default_value' => variable_get('os2dagsorden_budget_committee', FALSE),
  );
  $form['os2dagsorden_sidepane_menu_layout']['os2dagsorden_budgetforhandling_button_committees'] = array(
    '#type' => 'textfield',
    '#title' => t('Udvalg, hvis medlemmer har adgang til knappen <b>Budgetforhandlingt</b> i sidepanelet'),
    '#description' => t('Kommasepareret liste over udvalg, hvis medlemmer har adgang til knappen <b>Budgetforhandlingt</b> i sidepanelet'),
    '#default_value' => variable_get('os2dagsorden_budgetforhandling_button_committees', FALSE),
  );

  // Meeting import settings.
  $form['os2dagsorden_meeting_import'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('General meeting import settings'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_participants_delimeter'] = array(
    '#type' => 'textfield',
    '#title' => t('Participants delimeter sign'),
    '#default_value' => variable_get('os2dagsorden_participants_delimeter', ', '),
    '#description' => t('Sign with which the participants of the meeting will be separated. Is used in the meeting view.<br/> E.g. if delimeter is <b>coma (,)</b> the list would look like:<br/>
    Jørgen Andersen, Nich Bendtsen, Poul Møller, ...<br/><br/>HTML tags (such as &lt;br/&gt;) can be used.'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_send_notification_committee'] = array(
    '#type' => 'textfield',
    '#title' => t('Send notification of committees'),
    '#maxlength' => 10000,
    '#default_value' => variable_get('os2dagsorden_send_notification_committee', FALSE),
    '#description' => t('Comma-separated list of committees changing status of which should send an email notification.'),
    '#autocomplete_path' => 'get_committee/autocomplete',
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_send_email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting notification email text'),
    '#description' => t('The email subject, which would be send to users interested in meeting publishing - Member / Follow+ / Follow (optional)'),
    '#default_value' => variable_get('os2dagsorden_send_email_subject', '!meeting_type til !committee er publiceret'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_send_email_body'] = array(
    '#type' => 'textarea',
    '#title' => t('Meeting notification email text'),
    '#description' => t('The email body, which would be send to users interested in meeting publishing - Member / Follow+ / Follow (optional)'),
    '#default_value' => variable_get('os2dagsorden_send_email_body', 'Til !user' . PHP_EOL . PHP_EOL .
      'Du abonnerer på !committee.' . PHP_EOL . 'Der er publiceret !meeting_type til !meeting_name !meeting_date.'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_send_email_text_available_variables'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => t('Can be used both in subject and body:') . '<br/>
    <b>!committee</b> - ' . t('name of the committee') . '<br/>
    <b>!meeting_name</b> - ' . t('title of the meeting') . '<br/>
    <b>!meeting_date</b> - ' . t('date of the meeting') . '<br/>
    <b>!meeting_type</b> - ' . t('type of the meeting with acticle: <i>et referat</i> or <i>en dagsorden</i>') . '<br/>
    <b>!meeting_link</b> - ' . t('link to the meeting') . '<br/>
    <b>!user</b> - ' . t('name of the user that message is sent to') . '<br/>',
    '#title' => t('Available variables'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_follows_committee_send_notification'] = array(
    '#type' => 'checkbox',
    '#title' => t('User that are just Follow the committee are getting notifications'),
    '#description' => t('Decide whether user that are just Follow the committee are getting notifications about new messages appeared on that committee.'),
    '#default_value' => variable_get('os2dagsorden_follows_committee_send_notification', FALSE),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_send_timespan_of_notification'] = array(
    '#type' => 'textfield',
    '#title' => t('Meeting should be not older than:'),
    '#maxlength' => 200,
    '#field_suffix' => t('Days'),
    '#size' => 10,
    '#description' => t('If meeting is not older than the set amount of day, notification is sent. <br/>Otherwise notification process is skipped'),
    '#default_value' => variable_get('os2dagsorden_send_timespan_of_notification', 10),
  );

  $form['os2dagsorden_meeting_import']['os2dagsorden_add_type_prefix'] = array(
    '#type' => 'radios',
    '#title' => t('Add prefix to Bilag/Sag bullet point attachments'),
    '#options' => array('true' => t('Add <i>Bilag/Sag</i> prefix'), 'false' => t('Add no prefix')),
    '#description' => t('This decides whether the will be added a prefix to Bilag or Case bullet point attachment on import.<br/>e.g. <b>Bilag:</b> Indsigelser of Bemaerkninger.'),
    '#default_value' => variable_get('os2dagsorden_add_type_prefix', 'true'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_import_full_agenda_as_attachments'] = array(
    '#type' => 'checkbox',
    '#title' => t('Importer agenda-filer som punkter'),
    '#description' => t('Afgør, om agenda-filer vil blive tilføjet til punkter som vedhæftede filer.'),
    '#default_value' => variable_get('os2dagsorden_import_full_agenda_as_attachments', FALSE),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_general_bullet_point_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Titler på punkter'),
    '#maxlength' => 200,
    '#default_value' => variable_get('os2dagsorden_general_bullet_point_title', t('Generalt')),
     '#states' => array(
      'visible' => array(
        ':input[name="os2dagsorden_import_full_agenda_as_attachments"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_general_bullet_full_doc_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Titel til bilag, der indeholder fuld dagsorden'),
    '#maxlength' => 200,
    '#default_value' => variable_get('os2dagsorden_general_bullet_full_doc_title', t('Åben samlet dokument')),
     '#states' => array(
      'visible' => array(
        ':input[name="os2dagsorden_import_full_agenda_as_attachments"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_general_bullet_full_doc_with_attach_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Titel til bilag, der indeholder fuld dagsorden med bilag'),
    '#maxlength' => 200,
    '#default_value' => variable_get('os2dagsorden_general_bullet_full_doc_with_attach_title', t('Åben samlet dokument med bilag')),
     '#states' => array(
      'visible' => array(
        ':input[name="os2dagsorden_import_full_agenda_as_attachments"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_filter_all_tags'] = array(
    '#type' => 'checkbox',
    '#title' => t('Filter all HTML tags'),
    '#default_value' => variable_get('os2dagsorden_filter_all_tags', 0),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_filtered_tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Filtered HTML tags lists'),
    '#maxlength' => 250,
    '#default_value' => variable_get('os2dagsorden_filtered_tags', ''),
    '#description' => t('Comma-separated list of html tags that want to filter out (fe. p,h1,h2).'),
    '#states' => array(
        'disabled' => array(
          '[name="os2dagsorden_filter_all_tags"]' => array('checked' => TRUE),
        ),
      ),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_stripped_tags'] = array(
    '#type' => 'textfield',
    '#title' => t('Striped HTML tags lists'),
    '#maxlength' => 250,
    '#default_value' => variable_get('os2dagsorden_stripped_tags', ''),
    '#description' => t('Comma-separated list of html tags that want to strip (fe. font, span).'),
   );
  $form['os2dagsorden_meeting_import']['os2dagsorden_allowed_span_styles'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed styles in filtered span tags'),
    '#maxlength' => 250,
    '#default_value' => variable_get('os2dagsorden_allowed_span_styles', ''),
    '#description' => t('Comma-separated list of allowed styles in <span> tag (fe. font-weight:bold)'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_tab_tag'] = array(
    '#type' => 'textfield',
    '#title' => t('Replace  multiple &amp;nbsp; tags with:'),
    '#maxlength' => 250,
    '#default_value' => variable_get('os2dagsorden_tab_tag', '&nbsp;'),
    '#description' => t('Tag what should be put into the document instead of multiple &amp;nbsp; tags'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_url_strip_count'] = array(
    '#type' => 'textfield',
    '#title' => t('Link (a href) limit:'),
    '#maxlength' => 250,
    '#default_value' => variable_get('os2dagsorden_url_strip_count', '20'),
    '#description' => t('Specify a max limit in chars, which a link can have. If the link has more than specified, then link text is trimmed and ... is added.'),
  );
  $form['os2dagsorden_meeting_import']['os2dagsorden_clean_image_tags'] = array(
    '#type' => 'checkbox',
    '#title' => t('Clean image (img) tags'),
    '#default_value' => variable_get('os2dagsorden_clean_image_tags', TRUE),
    '#description' => t('This will decide whether the image (img) tags scr will be removed during the import. The scr is checked for http headers, and if they are 404/FALSE the scr attribute is removed. However on some installation that should be omitted.'),
  );

  // PDF conversion settings.
  if (module_exists('os2web_pdf_conversion_manager')) {
    $form['os2dagsorden_file_import'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('* > PDF converting configuration'),
    );
    $form['os2dagsorden_file_import']['os2web_copied_filetype'] = array(
      '#type' => 'textfield',
      '#title' => t('Type of files to be converted when importing:'),
      '#maxlength' => 200,
      '#description' => t('Comma-separated list of file types. Example: xls,doc,docx,xlsx'),
      '#default_value' => variable_get('os2web_copied_filetype', OS2WEB_PDF_CONVERSION_MANAGER_COPIED_FILETYPE),
    );
    $form['os2dagsorden_file_import']['os2web_convertion_dir'] = array(
      '#type' => 'textfield',
      '#title' => t('Directory to send the files for converting:'),
      '#maxlength' => 200,
      '#description' => t('This is the path where files copy for converting'),
      '#default_value' => variable_get('os2web_convertion_dir', OS2WEB_PDF_CONVERSION_MANAGER_CONVERSION_DIR),
      '#element_validate' => array('_os2dagsorden_os2web_convertion_dir_validate'),
    );
  }
  if (module_exists('os2dagsorden_pdf2htmlex')) {
    $form['os2dagsorden_pdf2htmlex_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('PDF > HTML Converter settings'),
    );
    $form['os2dagsorden_pdf2htmlex_settings']['os2dagsorden_pdf2htmlex_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to pdf2htmlEX'),
      '#description' => t('Path to pdf2htmlEX'),
      '#default_value' => variable_get('os2dagsorden_pdf2htmlex_path', ""),
      '#element_validate' => array('_os2dagsorden_pdf2htmlex_validate'),
    );
    $form['os2dagsorden_pdf2htmlex_settings']['pdf2html_rendering_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path for HTML files to be rendered'),
      '#default_value' => variable_get('pdf2html_rendering_path', ''),
      '#description' => t('Relative path for HTML files to be rendered by os2dagsorden_pdf2htmlEX module. If empty, the path with the original PDF files will be used.<br/><b>Important!</b> You will need to create the path yourself.'),
      '#element_validate' => array('_os2dagsorden_pdf2html_rendering_path_validate'),
    );
  }

  if (module_exists('os2dagsorden_gs_conversion')) {
    $form['os2dagsorden_gs_conversion_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('GhostScript (GS) converter settings'),
    );
    $form['os2dagsorden_gs_conversion_settings']['os2dagsorden_gs_conversion_gs_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to GhostScript (GS)'),
      '#description' => t('Local path to GhostScript (GS)'),
      '#default_value' => variable_get('os2dagsorden_gs_conversion_gs_path', ""),
    );
    $form['os2dagsorden_gs_conversion_settings']['os2dagsorden_gs_conversion_symbols_cleanup'] = array(
      '#type' => 'textfield',
      '#title' => t('List of symbols that should be removed the filename'),
      '#description' => t('Comma-separated list of symbols that should be removed from the filename before GS conversion'),
      '#default_value' => variable_get('os2dagsorden_gs_conversion_symbols_cleanup', "–"),
    );
  }
  if (module_exists('os2dagsorden_pdf2png_conversion')) {
    $form['os2dagsorden_pdf2png_conversion_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Sedja Console converter settings'),
    );
    $form['os2dagsorden_pdf2png_conversion_settings']['os2dagsorden_pdf2png_conversion_sedja_console_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to Sejda-console'),
      '#default_value' => variable_get('os2dagsorden_pdf2png_conversion_sedja_console_path', ""),
    );
    $form['os2dagsorden_pdf2png_conversion_settings']['os2dagsorden_show_thumbnails_on_ipad'] = array(
      '#type' => 'checkbox',
      '#title' => t('Vis miniature side på Ipad'),
      '#description' => t('Absolute path to sejda-console'),
      '#default_value' => variable_get('os2dagsorden_show_thumbnails_on_ipad', FALSE),
    );
  }

  if (module_exists('os2dagsorden_automated_logout_configuration')) {
    $form['os2dagsorden_automated_logout_configuration_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Automated logout configuration settings'),
    );
    $form['os2dagsorden_automated_logout_configuration_settings']['os2dagsorden_automated_logout_show_autologout_inactivity_message'] = array(
      '#type' => 'checkbox',
      '#default_value' => variable_get('os2dagsorden_automated_logout_show_autologout_inactivity_message', TRUE),
      '#title' => t('Show autologout message'),
      '#description' => t('This decides whether a autologut message will be shown'),
    );
  }
  // Clear cache settings.
  $form['os2dagsorden_clear_cache_after_import'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Clear cache after import settings'),
  );
  $form['os2dagsorden_clear_cache_after_import']['os2dagsorden_clear_cache_active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Active clear cahce after importing meetings'),
    '#default_value' => variable_get('os2dagsorden_clear_cache_active', 0),
  );

  $form['os2dagsorden_clear_cache_after_import']['os2dagsorden_clear_cache_frequency'] = array(
    '#type' => 'select',
    '#title' => t('Clear cache frequency after pdf convert'),
    '#options' => array(
      '0' => t('Never'),
      '10' => t('Every 10 mins'),
      '30' => t('Every 30 mins'),
      '60' => t('Every hour'),
    ),
    '#default_value' => variable_get('os2dagsorden_clear_cache_frequency', 0),
  );

  $form['os2dagsorden_committee_category_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Committee category settings'),
  );
  $options2 = array('true' => t('Show'), 'false' => t('Do not show'));
  $form['os2dagsorden_committee_category_settings']['os2dagsorden_show_meeting_category'] = array(
    '#type' => 'radios',
    '#title' => t('Show meeting categories'),
    '#default_value' => variable_get('os2dagsorden_show_meeting_category'),
    '#options' => $options2,
    '#description' => t('This decides whether a meeting title in the calendar view will be prefixed with the meeting category (<b>[P]</b>olitical/<b>[O]</b>rganization)'),
  );
  $cat_voc = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_committee_category');
  $cat_terms = taxonomy_get_tree($cat_voc->vid);
  $cat_select = array();
  foreach ($cat_terms as $cat_term) {
    $cat_select[$cat_term->tid] = $cat_term->name;
  }
  $form['os2dagsorden_committee_category_settings']['os2dagsorden_user_default_committee_category'] = array(
    '#title' => t('Default value for user meeting category'),
    '#type' => 'checkboxes',
    '#description' => t('Select default value for user meeting category.'),
    '#options' => $cat_select,
    '#default_value' => variable_get('os2dagsorden_user_default_committee_category'),
  );
  $form['os2dagsorden_committee_category_settings']['os2dagsorden_committee_forbidden_allowed_categories_mode'] = array(
    '#type' => 'radios',
    '#default_value' => variable_get('os2dagsorden_committee_forbidden_allowed_categories_mode', 'forbidden'),
    '#title' => t('Committee forbidden categories mode'),
    '#options' => array(
      'forbidden' => t('Forbidden mode'),
      'allowed' => t('Allowed mode'),
    ),
    '#description' => t('Changes the user settings page by toggling between forbidden and allowed categories mode. In other words white list and black list.'),
  );
  $form['os2dagsorden_committee_category_settings']['os2dagsorden_committee_default_forbidden_cat'] = array(
    '#type' => 'checkboxes',
    '#default_value' => variable_get('os2dagsorden_committee_default_forbidden_cat'),
    '#options' => $cat_select,
    '#title' => "<b>" . t('Default forbidden categories') . "</b>",
    '#description' => t('Which categories the user is FORBIDDEN to have access to when created'),
  );

  // SOFD Integration settings.
  if (module_exists('os2dagsorden_importer')) {
    $form['os2dagsorden_pws_sofd_group'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('SOFD Integration settings'),
    );
    $form['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_use_local_copy'] = array(
      '#type' => 'checkbox',
      '#default_value' => variable_get('os2dagsorden_pws_use_local_copy'),
      '#title' => t('Use local XML files'),
      '#description' => variable_get('os2dagsorden_pws_use_local_copy') ? 'Diselect and click save to enable the fields below' : 'Selecting and clicking save will disable the textfields below',
      '#element_validate' => array('_os2dagsorden_sofd_local_copy_validate'),
    );
    $form['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_v_meetings_url'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('os2dagsorden_pws_v_meetings_url'),
      '#title' => t('Meeting (planned meetings) view url:'),
      '#maxlength' => 256,
      '#disabled' => variable_get('os2dagsorden_pws_use_local_copy'),
    );
    $form['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_v_meetinggroups_url'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('os2dagsorden_pws_v_meetinggroups_url'),
      '#title' => t('Meeting groups (is member of/follows/follows+) view url:'),
      '#maxlength' => 256,
      '#disabled' => variable_get('os2dagsorden_pws_use_local_copy'),
    );
    $form['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_v_acadregroups_url'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('os2dagsorden_pws_v_acadregroups_url'),
      '#title' => t('Acadre groups (committees) url:'),
      '#maxlength' => 256,
      '#disabled' => variable_get('os2dagsorden_pws_use_local_copy'),
    );
  }

  // Acadre service importer.
  if (module_exists('os2dagsorden_acadre_service_importer')) {
    $form['os2dagsorden_acadre_service_importer_placeholder'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Acadre Service importer Integration settings'),
    );
    $form['os2dagsorden_acadre_service_importer_placeholder']['os2dagsorden_acadre_service_meetings_url'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('os2dagsorden_acadre_service_meetings_url'),
      '#title' => t('Meeting (planned meetings) view url:'),
      '#maxlength' => 256,
    );
    $form['os2dagsorden_acadre_service_importer_placeholder']['os2dagsorden_acadre_service_committees_url'] = array(
      '#type' => 'textfield',
      '#default_value' => variable_get('os2dagsorden_acadre_service_committees_url'),
      '#title' => t('Committees url:'),
      '#maxlength' => 256,
    );
  }

  $form['os2dagsorden_help_text_placeholder'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Help-button texts'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_upcoming_meetings_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_upcoming_meetings_help_text', 'Her vises de kommende 5 møder i "Mine udvalg". Det vil sige udvalg som man enten er medlem af eller følger. Udvalgene kan ses i boksene "Medlem af" og "Jeg følger" i højremenuen. Der er ikke en dagsorden tilgængelig før status er angivet til Dagsorden eller Dagsorden+.'),
    '#title' => t('Coming meeting help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_meetings_search_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_meetings_search_help_text', 'Her vises de møder der indeholdt møder, punkter eller bilag der matchede dine søgekriterier. Søgeresultatet vises på mødeniveau i listen.'),
    '#title' => t('Meetings search page help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_meetings_search_block_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_meetings_search_block_help_text', 'Her kan der søges i dagsordenspunkters tekst og i bilagenes fritekst, hvis sidstnævnte er konverteret til html. Søgning kan filtreres via dato, udvalg og søgeord.'),
    '#title' => t('Meetings search block help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_last_8_meetings_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_last_8_meetings_help_text', 'Her vises de seneste 8 møder i alle mødefora. Der er ikke et referat tilgængeligt før status er angivet til Referat eller Referat+.'),
    '#title' => t('Last 8 meetings help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_annotator_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_annotator_help_text', 'Hvis du ønsker at lave en kommentar til et ord eller et afsnit, så marker blot dette hvorefter du får mulighed for at indtaste kommentaren.'),
    '#title' => t('Annotator page help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_calendar_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_calendar_help_text', 'Her findes en dags- eller månedskalender med links til dagsordner og referater hvis de er publicerede. Det er muligt at eksportere møderne til en fil, som så kan importeres til egen kalender, eks. i Outlook.'),
    '#title' => t('Calendar block help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_user_committees_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_user_committees_help_text', 'Her kan du se de udvalg du er medlem af. Ret henvendelse til Direktionssekretariatet hvis du mener det skal ændres.'),
    '#title' => t('User committees help text'),
  );

  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_user_follows_help_text'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_user_follows_help_text', 'Her kan du se de udvalg du er sat op til at følge. Følger betyder at du kun kan se åbne punkter og Følger+ betyder at du også kan se lukkede punkter. Ret henvendelse til Direktionssekretariatet hvis du mener det skal ændres.'),
    '#title' => t('User follows help text'),
  );
  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_calendar_replacement_block_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Print meeting calendar block text (asking person to turn his/her device horizontally)'),
    '#default_value' => variable_get('os2dagsorden_calendar_replacement_block_text', 'Vend venligst din iPad horisontalt'),
  );

  $form['os2dagsorden_terminology_placeholder'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Terminology settings'),
  );

  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('General'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_mine_indstillinger'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_general_mine_indstillinger', 'Mine indstillinger'),
    '#title' => t('Log af'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_create_meeting'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_general_create_meeting', 'Tilføj møde'),
    '#title' => t('Log af'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_log_off'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_general_log_off', 'Log af'),
    '#title' => t('Log af'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_search'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_general_search', 'Søg'),
    '#title' => t('Søg'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_next_page'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_general_next_page', 'Næste'),
    '#title' => t('Næste'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_prev_page'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_general_prev_page', 'Forrige'),
    '#title' => t('Forrige'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_hide_logout_message'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('os2dagsorden_title_general_hide_logout_message', FALSE),
    '#title' => t('Hide Logout message'),
  );

  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Front page'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_coming_meetings'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_coming_meetings', 'Kommende møder i mine udvalg'),
    '#title' => t('View: Kommende møder i mine udvalg'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_last_8_my_meetings'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_last_8_my_meetings', 'Seneste 8 møder i mine udvalg'),
    '#title' => t('View: Seneste 8 møder i mine udvalg'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_last_8_all_meetings'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_last_8_all_meetings', 'Seneste møder i alle udvalg'),
    '#title' => t('View: Seneste møder i alle udvalg'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_calendar'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_calendar', 'Kalender'),
    '#title' => t('Block: Kalender'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_member_of'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_member_of', 'Medlem af'),
    '#title' => t('Block: Medlem af'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_following'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_following', 'Jeg følger'),
    '#title' => t('Block: Jeg følger'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_account_settings'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_account_settings', 'Konto indstillinger'),
    '#title' => t('Menu: Konto indstillinger'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_home'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_home', 'Forsiden'),
    '#title' => t('Menu: Forsiden'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_halfyear_calendar'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_frontpage_halfyear_calendar', 'Halvårs-kalender'),
    '#title' => t('Menu: Halvårs Kalender'),
  );

  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Meeting'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_status'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_status', 'Status'),
    '#title' => t('Status'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_meeting'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_meeting', 'Møde'),
    '#title' => t('Møde'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_date'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_date', 'Dato'),
    '#title' => t('Dato'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_location'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_location', 'Lokation'),
    '#title' => t('Lokation'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_freetext_placing'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_freetext_placing', 'Placering'),
    '#title' => t('Placering'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_comments'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_comments', 'Bemærkninger'),
    '#title' => t('Bemærkninger'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_special_comments'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_special_comments', 'Særlige forhold'),
    '#title' => t('Særlige forhold'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_participants'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_participants', 'Deltagere'),
    '#title' => t('Deltagere'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_referent'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_referent', 'Referent'),
    '#title' => t('Referent'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_cancelled_participants'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_cancelled_participants', 'Afbud / Fraværende'),
    '#title' => t('Afbud / Fraværende'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_agenda'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_agenda', 'Dagsorden:'),
    '#title' => t('Dagsorden'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_closed_agenda'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_closed_agenda', 'Lukket Dagsorden:'),
    '#title' => t('Lukket Dagsorden'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_committee'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_committee', 'Udvalg'),
    '#title' => t('Udvalg'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_see_all_speakerpapers'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_see_all_speakerpapers', 'Se alle talepapirer'),
    '#title' => t('Se alle talepapirer'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_print_all_speakerpapers'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_print_all_speakerpapers', 'Print alle talepapirer'),
    '#title' => t('Print alle talepapirer'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_meeting_notes_title'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_meeting_notes_title', 'Mødernoter'),
    '#title' => t('Egne noter'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_new_speaker_paper'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('os2dagsorden_title_meeting_new_speaker_paper', 'Nyt talepapir'),
    '#title' => t('Nyt talepapir'),
  );
  $form['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_new_speaker_paper_hide'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('os2dagsorden_title_meeting_new_speaker_paper_hide', FALSE),
    '#title' => t('Hide New speaker paper text'),
  );
  $form['os2dagsorden_help_text_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_anonymiseret'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#default_value' => variable_get('os2dagsorden_title_anonymiseret', 'Anonymiseret'),
    '#title' => t('Title for Anonymous bullet point or bilag'),
  );

  // Annotator settings.
  $form['os2dagsorden_annotator_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Annotator settings'),
  );
  $form['os2dagsorden_annotator_settings']['os2dagsorden_annotator_settings_save_notes'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('os2dagsorden_annotator_settings_save_notes', TRUE),
    '#title' => t('Save annotation notes when meeting deleting'),
  );

  // Upload quide PDF file.
  $form['upload_guide'] = array(
    '#type' => 'fieldset',
    '#title' => t('Quick quide PDF file'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['upload_guide']['quick_guide_upload_fid'] = array(
    '#title' => t('Choose a file'),
    '#type' => 'managed_file',
    '#default_value' => variable_get('quick_guide_upload_fid', ''),
    '#description' => t('Remember to save this form.'),
    '#upload_location' => 'public://PDF/',
  );
  // Upload quide PDF file.
  $form['os2dagsorden_right_sidebar_arrow_position'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sidepane arrow position'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['os2dagsorden_right_sidebar_arrow_position']['os2dagsorden_right_sidebar_arrow_position_radios'] = array(
    '#title' => t('Sidepane arrow position'),
    '#type' => 'radios',
    '#options' => array('classic' => 'classic', 'equal_height' => 'equal height'),
    '#default_value' => variable_get('os2dagsorden_right_sidebar_arrow_position_radios', 'classic'),
    '#description' => t('Default position or equal height with meeting blocks.'),
  );

  // Meeting calendar view settings.
  $form['os2dagsorden_meeting_view_calendar_settings'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Meeting view calendar settings'),
  );
  $form['os2dagsorden_meeting_view_calendar_settings']['os2dagsorden_meeting_view_calendar_short_name'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use committee short instead of the meeting name'),
    '#description' => t('This decides whether a committee short name will used instead of meeting name in the calendar view'),
    '#default_value' => variable_get('os2dagsorden_meeting_view_calendar_short_name', FALSE),
  );
  $form['os2dagsorden_meeting_view_calendar_settings']['os2dagsorden_meeting_show_calendar_block_on_cal_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show calendar block on calendar pages'),
    '#description' => t('This decides whether calendar block will be shown even on the calendar pages (month/day views)'),
    '#default_value' => variable_get('os2dagsorden_meeting_show_calendar_block_on_cal_pages', FALSE),
  );

  // Token login settings.
  if (module_exists('os2dagsorden_token_login')) {
    $form['os2dagsorden_token_login_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Token login settings'),
    );
    $form['os2dagsorden_token_login_settings']['os2dagsorden_token_login_ip_whitelist'] = array(
      '#type' => 'textfield',
      '#title' => t('IP whitelist'),
      '#description' => t('Comma separated list of IP that can login via the token login. Partial IPs can be used'),
      '#default_value' => variable_get('os2dagsorden_token_login_ip_whitelist'),
    );
    $form['os2dagsorden_token_login_settings']['os2dagsorden_token_login_allowed_paths'] = array(
      '#type' => 'textarea',
      '#title' => t('Allowed paths'),
      '#description' => t('List paths that can be accessed after logging in with token. Each path must be on single line. Leave empty to not restrict any paths.'),
      '#default_value' => variable_get('os2dagsorden_token_login_allowed_paths', '<front>'),
    );
  }

  // Settings for meetings in external committee.
  $form['os2dagsorden_external_committee'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('External committee'),
  );
  $form['os2dagsorden_external_committee']['os2dagsorden_timespan_open_meeting_content'] = array(
    '#type' => 'textfield',
    '#title' => t('Participants must see the contents of the meeting before:'),
    '#maxlength' => 200,
    '#field_suffix' => t('Days'),
    '#size' => 10,
    '#description' => t('Number of days before the meeting for which participants should see its contents'),
    '#default_value' => variable_get('os2dagsorden_timespan_open_meeting_content', 14),
  );
  $form['os2dagsorden_external_committee']['os2dagsorden_external_committee_anonymize_button_behavior'] = array(
    '#type' => 'radios',
    '#title' => t('For user created meetings the "delete / anonymize" button will:'),
    '#options' => array('delete' => 'delete', 'anonymize' => 'anonymize'),
    '#default_value' => variable_get('os2dagsorden_external_committee_anonymize_button_behavior', 'anonymize'),
    '#description' => t('This decides what "delete / anonymize" button will to do: clear meeting content or delete meeting'),
  );


 if (module_exists('os2dagsorden_create_agenda')) {
    // Settings for plupupload
    $form['os2dagsorden_plupupload_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Plupupload configuration'),
    );
    $form['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_autoupload'] = array(
      '#type' => 'checkbox',
      '#title' => t('Automatically start upload when files added'),
      '#default_value' => variable_get('os2dagsorden_plupupload_autoupload', TRUE),
    );
    $form['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_allowed_filetypes'] = array(
      '#type' => 'textfield',
      '#title' => t('Allowed file types:'),
      '#description' => t('Separate extensions with a space and do not include the leading dot. Fe. "doc docx pdf"'),
      '#default_value' => variable_get('os2dagsorden_plupupload_allowed_filetypes', 'doc docx pdf'),
    );
    $form['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_max_filesize'] = array(
      '#type' => 'textfield',
      '#title' => t('Max file size:'),
      '#field_suffix' => 'MB',
      '#size' => 10,
      '#description' => t('Maximum allowed filesize'),
      '#default_value' => variable_get('os2dagsorden_plupupload_max_filesize', 2),
    );
    $form['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_filedir_uri'] = array(
      '#type' => 'textfield',
      '#title' => t('File directory for uploaded files'),
      '#description' => t('URI to folder where uploaded files sroted'),
      '#default_value' => variable_get('os2dagsorden_plupupload_filedir_uri', 'private://manual_agendas'),
      '#element_validate' => array('_os2dagsorden_plupload_upload_dir_validate'),
    );
  }
  if (module_exists('os2dagsorden_create_agenda')) {
    $form['os2dagsorden_create_agenda_settings'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Manual create agenda configuration'),
    );
    $form['os2dagsorden_create_agenda_settings']['os2dagsorden_create_agenda_period_to_store_latest_created_meeting_data'] = array(
      '#type' => 'textfield',
      '#title' => t('Save latest created meetings data during:'),
      '#field_suffix' => t('hours'),
      '#size' => 10,
      '#description' => t('The period in hours during what the data of latest created meeting  will be save and will be populated during new meeting creation. -1 for indefinite.'),
      '#default_value' => variable_get('os2dagsorden_create_agenda_period_to_store_latest_created_meeting_data', -1),
    );
    $form['os2dagsorden_create_agenda_settings']['os2dagsorden_create_agenda_meeting_start_date_default'] = array(
      '#type' => 'textfield',
      '#title' => t('Antall dage i fremtiden mødet skal oprettes'),
      '#field_suffix' => t('days'),
      '#size' => 10,
      '#description' => t('The amount of days in future for meeting start date default value. If that field is set to 10, when a meeting created start date will be set 10 days ahead.'),
      '#default_value' => variable_get('os2dagsorden_create_agenda_meeting_start_date_default'),
    );
    $form['os2dagsorden_create_agenda_settings']['os2dagsorden_create_agenda_full_agenda_upload_fields_edit_form'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show fields for upload PDF files with full agenda on meeting creation form'),
      '#description' => t('This decides whether fields for upload PDF files with full agenda will be added to meeting creation form'),
      '#default_value' => variable_get('os2dagsorden_create_agenda_full_agenda_upload_fields_edit_form', FALSE),
    );
  }
  // Sort items alphabetically.
  ksort($form);
  return system_settings_form($form);
}

/**
 * Implements hook_theme_settings().
 */
function os2dagsorden_settings_theme_settings() {
  $theme = variable_get('theme_default');
  drupal_goto('admin/appearance/settings/' . $theme);
}

/**
 * Validates pdf2htmlEX installation.
 *
 * By checking if which commands returns anything.
 *
 * @param mixed $element
 *   The form element.
 * @param mixed $form_state
 *   The state of form.
 */
function _os2dagsorden_pdf2htmlex_validate($element, &$form_state) {
  $value = $element['#value'];
  if (!empty($value)) {
    $exec = shell_exec("which " . $value);
    if (empty($exec)) {
      form_error($element, "Incorrect pdf2htmlEX path");
      watchdog("os2dagsorden", 'Incorrect pdf2htmlEX path');
    }
  }
}

/**
 * Validates URL responce.
 *
 * If response code not 200, form error is set.
 *
 * @param mixed $element
 *   The form element.
 * @param mixed $form_state
 *   The state of form.
 */
function _os2dagsorden_url_responce_validate($element, &$form_state) {
  $value = $element['#value'];

  if ($value != "") {
    $url = (strpos($value, "http://") !== 0) ? 'http://' . $value : $value;

    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_NOBODY, TRUE);
    curl_exec($ch);
    $retcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

    if ($retcode != 200) {
      form_error(
        $element,
        t("Specified url !url does not respond. Error code: !retcode", array(
          '!url' => $url,
          '!retcode' => $retcode,
        ))
      );
    }
    curl_close($ch);
  }
}

/**
 * Checks that files for SOFD are present.
 *
 * File paths are: "public://acadre_views/V_web_MeetingGroups.xml",
 * "public://acadre_views/V_web_AcadreGroups.xml",
 * "public://acadre_views/V_web_Meetings.xml".
 *
 * @param mixed $element
 *   The form element.
 * @param mixed $form_state
 *   The state of form.
 */
function _os2dagsorden_sofd_local_copy_validate($element, &$form_state) {
  $value = $element['#value'];
  // If checked.
  if ($value == 1) {
    if (is_dir("public://acadre_views")) {
      // Check files in directory.
      $files = array("public://acadre_views/V_web_MeetingGroups.xml",
        "public://acadre_views/V_web_AcadreGroups.xml",
        "public://acadre_views/V_web_Meetings.xml",
      );

      foreach ($files as $file) {
        if (!file_exists($file)) {
          form_error($element, t("File !file does not exist.", array('!file' => $file)));
        }
      }

    }
    else {
      form_error($element, t("directory public://acadre_views does not exist."));
    }

  }
}

/**
 * Validates that the pdf rendering path is present and writable.
 *
 * @param mixed $element
 *   The form element.
 * @param mixed $form_state
 *   The state of form.
 */
function _os2dagsorden_pdf2html_rendering_path_validate($element, &$form_state) {
  $value = $element['#value'];
  if ($value != "") {
    if (is_dir($value)) {
      if (!is_writable($value)) {
        form_error($element, t("Could not writte to directory. Please check permissions"));
      }
    }
    else {
      form_error($element, t("Directory does not exist"));
    }
  }
}

/**
 * Validates that the os2web conversion path is present and writable.
 *
 * @param mixed $element
 *   The form element.
 * @param mixed $form_state
 *   The state of form.
 */
function _os2dagsorden_os2web_convertion_dir_validate($element, &$form_state) {
  $value = $element['#value'];
  if (file_exists($value)) {
    if (!is_writable($value)) {
      form_error($element, t("Could not writte to directory. Please check permissions"));
    }
  }
  else {
    form_error($element, t("Directory !value does nor exist", array('!value' => $value)));
  }
}

/**
 * Validates that the file upload dir is present and writable.
 *
 * @param mixed $element
 *   The form element.
 * @param mixed $form_state
 *   The state of form.
 */
function _os2dagsorden_plupload_upload_dir_validate($element, &$form_state) {
  $value = $element['#value'];
  if (file_exists($value)) {
    if (!is_writable($value)) {
      form_error($element, t("Could not writte to directory. Please check permissions"));
    }
  }
  else {
    form_error($element, t("Directory !value does nor exist", array('!value' => $value)));
  }
}

/**
 * Sets the variables to the ones imported from the file.
 *
 * @param mixed $form
 *   The settings form.
 * @param mixed $form_state
 *   The state of the form.
 */
function os2dagsorden_settings_import_from_file($form, &$form_state) {
  if (!$form_state['values']['os2dagsorden_settings_file']) {
    drupal_set_message(t('No file has beed added'), 'error');
  }
  else {
    drupal_set_message(t('Imported settings from file'));

    $file = file_load($form_state['values']['os2dagsorden_settings_file']);
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);

    variable_set('os2dagsorden_settings_file', $file->fid);
    $content = file_get_contents(drupal_realpath($file->uri));
    file_put_contents(drupal_realpath($file->uri), trim($content));
    $setting_array = parse_ini_file(drupal_realpath($file->uri), TRUE);

    variable_set('os2dagsorden_text_size', $setting_array['os2dagsorden_text_layout']['os2dagsorden_body_text_size']);
    variable_set('os2dagsorden_text_size', $setting_array['os2dagsorden_text_layout']['os2dagsorden_title_text_size']);

    variable_set('os2dagsorden_show_meeting_unpublish_button', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_unpublish_button']);
    variable_set('os2dagsorden_show_import_form_button', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_import_form_button']);
    variable_set('os2dagsorden_show_meeting_committee', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_committee']);
    variable_set('os2dagsorden_show_meeting_enddate', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_enddate']);
    variable_set('os2dagsorden_show_meeting_type', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_meeting_type']);
    variable_set('os2dagsorden_show_participants_afbud', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_participants_afbud']);
    variable_set('os2dagsorden_show_print_icon', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_print_icon']);
    variable_set('os2dagsorden_show_send_to_friend_icon', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_send_to_friend_icon']);
    variable_set('os2dagsorden_show_download_description_button_on_bp_view', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_download_description_button_on_bp_view']);
    variable_set('os2dagsorden_show_generate_pdf_link', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_generate_pdf_link']);
    variable_set('os2dagsorden_show_generate_pdf_link_full', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_generate_pdf_link_full']);
    variable_set('os2dagsorden_show_bullet_case_id_link', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bullet_case_id_link']);
    variable_set('os2dagsorden_bullet_case_id_link_url', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_bullet_case_id_link_url']);
    variable_set('os2dagsorden_show_comments', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_comments']);
    variable_set('os2dagsorden_show_closed_bullet_point_content', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_closed_bullet_point_content']);
    variable_set('os2dagsorden_open_closed_bullet_in_new_tab', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_open_closed_bullet_in_new_tab']);
    variable_set('os2dagsorden_show_closed_bullet_content_depending_of_publishlevel', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_closed_bullet_content_depending_of_publishlevel']);
    variable_set('os2dagsorden_show_bilag', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bilag']);
    variable_set('os2dagsorden_sager_group_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_sager_group_title ']);
    variable_set('os2dagsorden_use_abbr', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_use_abbr']);
    variable_set('os2dagsorden_show_bp_comname', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bp_comname']);
    variable_set('os2dagsorden_show_bilag_pages_amount', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bilag_pages_amount']);
    variable_set('os2dagsorden_show_group_pager_on_bpa_page', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_group_pager_on_bpa_page']);
    variable_set('os2dagsorden_meeting_view_settings_not_attach_addtional_agenda', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_view_settings_not_attach_addtional_agenda']);
    variable_set('os2dagsorden_show_bp_title_system_text', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_bp_title_system_text']);

// FIXME: new variable.
    variable_set('os2dagsorden_pdf_download_whitelist_enabled', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_pdf_download_whitelist_enabled']);
    variable_set('os2dagsorden_pdf_download_whitelist', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_pdf_download_whitelist']);
    variable_set('os2dagsorden_sager_group_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_sager_group_title']);
    variable_set('os2dagsorden_group_closed_bullet_points', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_group_closed_bullet_points']);
    variable_set('os2dagsorden_hidden_agenda_committee', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_hidden_agenda_committee']);
    variable_set('os2dagsorden_users_accessed_hidden_agenda_committee', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_users_accessed_hidden_agenda_committee']);
    variable_set('os2dagsorden_meeting_description_download_dropdown', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_download_dropdown']);
    variable_set('os2dagsorden_meeting_description_download_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_download_title']);
    variable_set('os2dagsorden_meeting_description_download_title_additional', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_download_title_additional']);
    variable_set('os2dagsorden_meeting_description_with_att_download_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_with_att_download_title']);
    variable_set('os2dagsorden_meeting_description_with_att_download_title_additional', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_with_att_download_title_additional']);
    variable_set('os2dagsorden_meeting_description_generate_pdf_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_generate_pdf_title']);
    variable_set('os2dagsorden_meeting_description_generate_pdf_full_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_meeting_description_generate_pdf_full_title']);
    variable_set('os2dagsorden_create_note_text', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_create_note_text']);
    variable_set('os2dagsorden_search_switch_to_note_text', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_search_switch_to_note_text']);
    variable_set('os2dagsorden_create_note_hide_text', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_create_note_hide_text']);
    variable_set('os2dagsorden_create_note_text_use_title', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_create_note_text_use_title']);
    variable_set('os2dagsorden_notes_page_standart_layout', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_notes_page_layout']);
    variable_set('os2dagsorden_print_pdf_show_commitee', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_print_pdf_show_commitee']);
    variable_set('os2dagsorden_search_restrict_search_function', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_search_restrict_search_function']);
    variable_set('os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_pdf_download_enabled_both_closed_open_agendas_links']);
    variable_set('os2dagsorden_hide_meeting_details', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_hide_meeting_details']);
    variable_set('os2dagsorden_expand_all_bullets', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_all_bullets']);
    variable_set('os2dagsorden_expand_attachment_onload', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_attachment_onload']);
    variable_set('os2dagsorden_expand_bilags', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_bilags']);
    variable_set('os2dagsorden_expand_cases', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_expand_cases']);
    variable_set('os2dagsorden_show_massive_expand_collapse_button', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_show_massive_expand_collapse_button']);
    variable_set('os2dagsorden_show_bilag_bullet_point', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_show_bilag_bullet_point']);
    variable_set('os2dagsorden_show_all_bilags_list_on_attachment_page', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_show_all_bilags_list_on_attachment_page']);
    variable_set('os2dagsorden_show_case_bullet_point', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_show_case_bullet_point']);
    variable_set('os2dagsorden_show_bullet_case_nr', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_show_bullet_case_nr']);
    variable_set('os2dagsorden_attachments_pages_per_view_desktop', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_desktop']);
    variable_set('os2dagsorden_attachments_pages_per_view_desktop_meeting_veiw', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_desktop_meeting_veiw']);
    variable_set('os2dagsorden_attachments_pages_per_view_mobile', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_mobile']);
    variable_set('os2dagsorden_attachments_pages_per_view_mobile_meeting_view', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_pages_per_view_mobile_meeting_view']);
    variable_get('os2dagsorden_attachments_show_scrollbar_on_attachment_page', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_show_scrollbar_on_attachment_page']);
    variable_set('os2dagsorden_show_expand_all_button', $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_show_expand_all_button']);
    variable_set('os2dagsorden_attachments_open_bilags_as_new_tab',  $setting_array['os2dagsorden_meeting_view_layout']['os2dagsorden_attachments_open_bilags_as_new_tab']);
    variable_set('os2dagsorden_show_user_no_roles_permissions', $setting_array['os2dagsorden_meeting_view_settings']['os2dagsorden_show_user_no_roles_permissions']);

    variable_set('os2dagsorden_latest_meetings_view_mode', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_latest_meetings_view_mode']);
    variable_set('os2dagsorden_meetings_views_last_8_show_meeting_title', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_title']);
    variable_set('os2dagsorden_meetings_views_last_8_show_meeting_committee', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_committee']);
    variable_set('os2dagsorden_meetings_views_last_8_show_meeting_location', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_location']);
    variable_set('os2dagsorden_meetings_views_last_8_show_meeting_is_closed', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_meeting_is_closed']);
    variable_set('os2dagsorden_meetings_views_last_8_meeting_title_len', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_meeting_title_len']);
    variable_set('os2dagsorden_meetings_views_last_8_meeting_location_len', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_meeting_location_len']);
    variable_set('os2dagsorden_meetings_views_last_8_show_external_committee', $setting_array['os2dagsorden_latest_meetings_view_settings']['os2dagsorden_meetings_views_last_8_show_external_committe']);
    variable_set('os2dagsorden_collapse_menu', $setting_array['os2dagsorden_frontpage_layout']['os2dagsorden_collapse_menu']);
    variable_set('os2dagsorden_collapse_menu_touch', $setting_array['os2dagsorden_frontpage_layout']['os2dagsorden_collapse_menu_touch']);
    variable_set('os2dagsorden_collapse_menu_touch_pages', $setting_array['os2dagsorden_frontpage_layout']['os2dagsorden_collapse_menu_touch_pages']);
    variable_set('os2dagsorden_show_search_block_title', $setting_array['os2dagsorden_frontpage_layout']['os2dagsorden_show_search_block_title']);
    variable_set('os2dagsorden_frontpage_layout_show_hide_zoom_buttons', $setting_array['os2dagsorden_frontpage_layout']['os2dagsorden_frontpage_layout_show_hide_zoom_buttons']);
    variable_set('os2dagsorden_meetings_search_show_meeting_committee', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_show_meeting_committee']);
    variable_set('os2dagsorden_meetings_search_show_meeting_location', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_show_meeting_location']);
    variable_set('os2dagsorden_meetings_search_show_meeting_status', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_show_meeting_status']);
    variable_set('os2dagsorden_meetings_search_include_add_agenda', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_search_include_add_agenda']);
    variable_set('os2dagsorden_search_startdate', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_search_startdate']);
    variable_set('os2dagsorden_search_enddate', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_search_enddate']);
    variable_set('os2dagsorden_search_meeting_is_closed', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_search_meeting_is_closed']);
    variable_set('os2dagsorden_meetings_overview_remember_filter_period', $setting_array['os2dagsorden_meeting_search_view_settings']['os2dagsorden_meetings_overview_remember_filter_period']);

    variable_set('os2dagsorden_budget_committee', $setting_array['os2dagsorden_sidepane_menu_layout']['os2dagsorden_budget_committee']);
    variable_set('os2dagsorden_budgetforhandling_button_committees', $setting_array['os2dagsorden_sidepane_menu_layout']['os2dagsorden_budgetforhandling_button_committees']);

    variable_set('os2dagsorden_participants_delimeter', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_participants_delimeter']);
    variable_set('os2dagsorden_send_notification_committee', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_send_notification_committee']);
    variable_set('os2dagsorden_send_email_subject', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_send_email_subject']);
    variable_set('os2dagsorden_send_email_body', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_send_email_body']);
    variable_set('os2dagsorden_follows_committee_send_notification', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_follows_committee_send_notification']);
    variable_set('os2dagsorden_send_timespan_of_notification', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_send_timespan_of_notification']);
    variable_set('os2dagsorden_add_type_prefix', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_add_type_prefix']);
    variable_set('os2dagsorden_clean_image_tags', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_clean_image_tags']);
    variable_set('os2dagsorden_filter_all_tags',  $setting_array['os2dagsorden_meeting_import']['os2dagsorden_filter_all_tags']);
    variable_set('os2dagsorden_filtered_tags', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_filtered_tags']);
    variable_set('os2dagsorden_stripped_tags', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_stripped_tags']);
    variable_set('os2dagsorden_allowed_span_styles', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_allowed_span_styles']);
    variable_set('os2dagsorden_tab_tag', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_tab_tag']);
    variable_set('os2dagsorden_url_strip_count', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_url_strip_count']);
    variable_set('os2dagsorden_import_full_agenda_as_attachments', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_import_full_agenda_as_attachments']);
    variable_get('os2dagsorden_general_bullet_full_doc_title', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_general_bullet_full_doc_title']);
    variable_get('os2dagsorden_general_bullet_full_doc_with_attach_title', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_general_bullet_full_doc_with_attach_title']);
    variable_get('os2dagsorden_general_bullet_point_title', $setting_array['os2dagsorden_meeting_import']['os2dagsorden_general_bullet_point_title']);

    variable_set('os2dagsorden_meeting_unpublished_state_subject', $setting_array['os2dagsorden_meeting_unpublished_state']['os2dagsorden_meeting_unpublished_state_subject']);
    variable_set('os2dagsorden_meeting_unpublished_state_body', $setting_array['os2dagsorden_meeting_unpublished_state']['os2dagsorden_meeting_unpublished_state_body']);

    if (module_exists('os2web_pdf_conversion_manager')) {
      variable_set('os2web_copied_filetype', $setting_array['os2dagsorden_file_import']['os2web_copied_filetype']);
      variable_set('os2web_convertion_dir', $setting_array['os2dagsorden_file_import']['os2web_convertion_dir']);
    }

    if (module_exists('os2dagsorden_pdf2htmlex')) {
      variable_set('os2dagsorden_pdf2htmlex_path', $setting_array['os2dagsorden_pdf2htmlex_settings']['os2dagsorden_pdf2htmlex_path']);
      variable_set('pdf2html_rendering_path', $setting_array['os2dagsorden_pdf2htmlex_settings']['pdf2html_rendering_path']);
    }

    if (module_exists('os2dagsorden_gs_conversion')) {
      variable_set('os2dagsorden_gs_conversion_gs_path', $setting_array['os2dagsorden_gs_conversion_settings']['os2dagsorden_gs_conversion_gs_path']);
      variable_set('os2dagsorden_gs_conversion_symbols_cleanup', $setting_array['os2dagsorden_gs_conversion_settings']['os2dagsorden_gs_conversion_symbols_cleanup']);
    }

    if (module_exists('os2dagsorden_pdf2png_conversion')) {
      variable_set('os2dagsorden_pdf2png_conversion_sedja_console_path', $setting_array['os2dagsorden_pdf2png_conversion_settings']['os2dagsorden_pdf2png_conversion_sedja_console_path']);
      variable_set('os2dagsorden_show_thumbnails_on_ipad', $setting_array['os2dagsorden_pdf2png_conversion_settings']['os2dagsorden_show_thumbnails_on_ipad']);
    }

    variable_set('os2dagsorden_show_meeting_category', $setting_array['os2dagsorden_committee_category_settings']['os2dagsorden_show_meeting_category']);
    variable_set('os2dagsorden_user_default_committee_category', $setting_array['os2dagsorden_committee_category_settings']['os2dagsorden_user_default_committee_category']);
    variable_set('os2dagsorden_committee_forbidden_allowed_categories_mode', $setting_array['os2dagsorden_committee_category_settings']['os2dagsorden_committee_forbidden_allowed_categories_mode']);
    variable_set('os2dagsorden_committee_default_forbidden_cat', $setting_array['os2dagsorden_committee_category_settings']['os2dagsorden_committee_default_forbidden_cat']);

    variable_set('os2dagsorden_clear_cache_active', $setting_array['os2dagsorden_clear_cache_after_import']['os2dagsorden_clear_cache_active']);
    variable_set('os2dagsorden_clear_cache_frequency', $setting_array['os2dagsorden_clear_cache_after_import']['os2dagsorden_clear_cache_frequency']);

    if (module_exists('os2dagsorden_importer')) {
      variable_set('os2dagsorden_pws_use_local_copy', $setting_array['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_use_local_copy']);
      variable_set('os2dagsorden_pws_v_meetings_url', $setting_array['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_v_meetings_url']);
      variable_set('os2dagsorden_pws_v_meetinggroups_url', $setting_array['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_v_meetinggroups_url']);
      variable_set('os2dagsorden_pws_v_acadregroups_url', $setting_array['os2dagsorden_pws_sofd_group']['os2dagsorden_pws_v_acadregroups_url']);
    }

    if (module_exists('os2dagsorden_acadre_service_importer')) {
      variable_set('os2dagsorden_acadre_service_meetings_url', $setting_array['os2dagsorden_acadre_service_importer_placeholder']['os2dagsorden_acadre_service_meetings_url']);
      variable_set('os2dagsorden_acadre_service_committees_url', $setting_array['os2dagsorden_acadre_service_importer_placeholder']['os2dagsorden_acadre_service_committees_url']);
    }

    variable_set('os2dagsorden_upcoming_meetings_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_upcoming_meetings_help_text']);
    variable_set('os2dagsorden_meetings_search_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_meetings_search_help_text']);
    variable_set('os2dagsorden_meetings_search_block_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_meetings_search_block_help_text']);
    variable_set('os2dagsorden_last_8_meetings_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_last_8_meetings_help_text']);
    variable_set('os2dagsorden_annotator_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_annotator_help_text']);
    variable_set('os2dagsorden_calendar_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_calendar_help_text']);
    variable_set('os2dagsorden_user_committees_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_user_committees_help_text']);
    variable_set('os2dagsorden_user_follows_help_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_user_follows_help_text']);
    variable_set('os2dagsorden_calendar_replacement_block_text', $setting_array['os2dagsorden_help_text_placeholder']['os2dagsorden_calendar_replacement_block_text']);

    variable_set('os2dagsorden_title_general_mine_indstillinger', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_mine_indstillinger']);
    variable_set('os2dagsorden_title_general_log_off', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_log_off']);
    variable_set('os2dagsorden_title_general_search', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_search']);
    variable_set('os2dagsorden_title_general_prev_page', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_prev_page']);
    variable_set('os2dagsorden_title_general_next_page', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_next_page']);
    variable_set('os2dagsorden_title_general_hide_logout_message', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_hide_logout_message']);
    variable_set('os2dagsorden_title_general_create_meeting', $settings_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_general_placeholder']['os2dagsorden_title_general_create_meeting']);

    variable_set('os2dagsorden_title_frontpage_coming_meetings', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_coming_meetings']);
    variable_set('os2dagsorden_title_frontpage_last_8_my_meetings', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_last_8_my_meetings']);
    variable_set('os2dagsorden_title_frontpage_last_8_all_meetings', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_last_8_all_meetings']);
    variable_set('os2dagsorden_title_frontpage_calendar', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_calendar']);
    variable_set('os2dagsorden_title_frontpage_member_of', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_member_of']);
    variable_set('os2dagsorden_title_frontpage_following', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_following']);
    variable_set('os2dagsorden_title_frontpage_account_settings', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_account_settings']);
    variable_set('os2dagsorden_title_frontpage_home', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_home']);
    variable_set('os2dagsorden_title_frontpage_halfyear_calendar', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_frontpage_placeholder']['os2dagsorden_title_frontpage_halfyear_calendar']);

    variable_set('os2dagsorden_title_meeting_status', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_status']);
    variable_set('os2dagsorden_title_meeting_meeting', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_meeting']);
    variable_set('os2dagsorden_title_meeting_date', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_date']);
    variable_set('os2dagsorden_title_meeting_location', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_freetext_placing']);
    variable_set('os2dagsorden_title_meeting_freetext_placing', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_location']);
    variable_set('os2dagsorden_title_meeting_comments', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_comments']);
    variable_set('os2dagsorden_title_meeting_special_comments', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_special_comments']);
    variable_set('os2dagsorden_title_meeting_participants', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_participants']);
    variable_set('os2dagsorden_title_meeting_cancelled_participants', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_cancelled_participants']);
    variable_set('os2dagsorden_title_meeting_agenda', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_agenda']);
    variable_set('os2dagsorden_title_meeting_closed_agenda', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_closed_agenda']);
    variable_set('os2dagsorden_title_meeting_committee', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_committee']);
    variable_set('os2dagsorden_title_meeting_print_all_speakerpapers', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_print_all_speakerpapers']);
    variable_set('os2dagsorden_title_meeting_see_all_speakerpapers', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_see_all_speakerpapers']);
    variable_set('os2dagsorden_title_meeting_new_speaker_paper', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_new_speaker_paper']);
    variable_set('os2dagsorden_title_meeting_new_speaker_paper_hide', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_new_speaker_paper_hide']);
    variable_set('os2dagsorden_meeting_notes_title', $setting_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_meeting_notes_title']);
    variable_set('os2dagsorden_title_meeting_referent', $settings_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_meeting_referent']);
    variable_set('os2dagsorden_title_anonymiseret', $settings_array['os2dagsorden_terminology_placeholder']['os2dagsorden_terminology_meeting_placeholder']['os2dagsorden_title_anonymiseret']);
    variable_set('os2dagsorden_meeting_view_calendar_short_name', $setting_array['os2dagsorden_meeting_view_calendar_settings']['os2dagsorden_meeting_view_calendar_short_name']);
    variable_set('os2dagsorden_meeting_show_calendar_block_on_cal_pages', $setting_array['os2dagsorden_meeting_view_calendar_settings']['os2dagsorden_meeting_show_calendar_block_on_cal_pages']);

    variable_set('os2dagsorden_token_login_ip_whitelist', $setting_array['os2dagsorden_token_login_settings']['os2dagsorden_token_login_ip_whitelist']);
    variable_set('os2dagsorden_token_login_allowed_paths', $setting_array['os2dagsorden_token_login_settings']['os2dagsorden_token_login_allowed_paths']);

    variable_set('os2dagsorden_external_committee_anonymize_button_behavior', $setting_array['os2dagsorden_external_committee']['os2dagsorden_external_committee_anonymize_button_behavior']);
    variable_set('os2dagsorden_timespan_open_meeting_content', $setting_array['os2dagsorden_external_committee']['os2dagsorden_timespan_open_meeting_content']);

    if (module_exists('os2dagsorden_automated_logout_configuration')) {
      variable_set('os2dagsorden_automated_logout_show_autologout_inactivity_message', $setting_array['os2dagsorden_automated_logout_configuration_settings']['os2dagsorden_automated_logout_show_autologout_inactivity_message']);
    }
    variable_set('quick_guide_upload_fid', $setting_array['upload_guide']['quick_guide_upload_fid']);
    variable_set('print_pdf_page_orientation', $setting_array['os2dagsorden_meeting_view_settings']['print_pdf_page_orientation']);
    variable_set('os2dagsorden_right_sidebar_arrow_position_radios', $setting_array['os2dagsorden_right_sidebar_arrow_position']['os2dagsorden_right_sidebar_arrow_position_radios']);

    variable_set('os2dagsorden_plupupload_autoupload', $setting_array['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_autoupload']);
    variable_set('os2dagsorden_plupupload_allowed_filetypes', $setting_array['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_allowed_filetypes']);
    variable_set('os2dagsorden_plupupload_max_filesize', $setting_array['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_max_filesize']);
    variable_set('os2dagsorden_plupupload_filedir_uri', $setting_array['os2dagsorden_plupupload_settings']['os2dagsorden_plupupload_filedir_uri']);

  }
  if (module_exists('os2dagsorden_create_agenda')) {
    variable_set('os2dagsorden_create_agenda_period_to_store_latest_created meeting_data', $setting_array['os2dagsorden_create_agenda_settings']['os2dagsorden_create_agenda_period_to_store_latest_created meeting_data']);
    variable_set('os2dagsorden_create_agenda_meeting_start_date_default', $setting_array['os2dagsorden_create_agenda_settings']['os2dagsorden_create_agenda_meeting_start_date_default']);
    variable_set('os2dagsorden_create_agenda_full_agenda_upload_fields_edit_form', $setting_array['os2dagsorden_create_agenda_settings']['os2dagsorden_create_agenda_full_agenda_upload_fields_edit_form']);
  }
  variable_set('os2dagsorden_annotator_settings_save_notes', $setting_array['os2dagsorden_annotator_settings']['os2dagsorden_annotator_settings_save_notes']);
}
