<?php

/**
 * @file
 * Os2dagsorden_content_modify.user.
 */

/**
 * User edit form.
 *
 * Generates user edit form and (if uid is not null) loads and sets the
 * parameters.
 *
 * Depending on the user rights form is either limited (with no right) of full
 * (with administer os2web right).
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The state of the form.
 * @param int $uid
 *   The id of the user to be modified.
 *
 * @return mixed
 *   The form.
 */
function os2dagsorden_content_modify_user_edit_form($form, $form_state, $uid) {
  $form[] = array(
    '#markup' => '<div class="node clearfix">',
  );

  $form['full_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full name:'),
    '#size' => 60,
    '#maxlength' => 128,
    '#description' => t('Is displayed in the top right corner.'),
  );

  $form['organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization:'),
    '#size' => 60,
    '#maxlength' => 128,

  );

  $form['user_external'] = array(
    '#access' => user_access('administer external users'),
    '#type' => 'checkbox',
    '#title' =>  t('External user'),
    '#default_value' => FALSE,
  );
  $cat_voc = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_committee_category');
  $cat_terms = taxonomy_get_tree($cat_voc->vid);
  $cat_select = array();
  foreach ($cat_terms as $cat_term) {
    $cat_select[$cat_term->tid] = $cat_term->name;
  }

  $form['calendar_pref'] = array(
    '#type' => 'checkboxes',
    '#field_prefix' => t('Which types of meetings should user see in the calendar'),
    '#options' => $cat_select,
    '#title' => "<b>" . t('Calendar preferences:') . "</b>",
  );

  if (variable_get('os2dagsorden_committee_forbidden_allowed_categories_mode', 'forbidden') === 'forbidden') {
    $form['forbidden_cat'] = array(
      '#access' => user_access('administer users'),
      '#type' => 'checkboxes',
      '#options' => $cat_select,
      '#title' => "<b>" . t('Forbidden categories:') . "</b>",
      '#description' => t('Which categories the user is FORBIDDEN to have access to'),
    );
  }
  else {
    $form['allowed_cat'] = array(
      '#access' => user_access('administer users'),
      '#type' => 'checkboxes',
      '#options' => $cat_select,
      '#title' => "<b>" . t('Allowed categories:') . "</b>",
      '#description' => t('Which categories the user is ALLOWED to have access to'),
    );
  }

  $form['user_can_send_closed_bp'] = array(
    '#access' => user_access('administer users'),
    '#type' => 'radios',
    '#title' => "<b>" . t('User can send to friend closed bullet points') . "</b>",
    '#default_value' => FALSE,
    '#options' => array(
      TRUE => t('Yes'),
      FALSE => t('No'),
    ),
  );

  $form['user_roles'] = array(
    '#access' => (user_access('administer users')),
    '#type' => 'checkboxes',
    '#options' => array(
      'os2dagsorden limited' => t('OS2Dagsorden limited') . '<i> ' . t('(Kan kun tilgå udvalg man er medlem af)') . '</i>',
      'administrator+' => t('Administrator+') . '<i> ' . t('(adgang til redigering af udvalg og brugere)') . '</i>',
      'administrator++' => t('Administrator++') . '<i> ' . t('(rettigheder til al konfiguration og alle settings)') . '</i>',
    ),
    '#title' => '<b>' . t('Brugerroller') . '</b>',
    '#description' => t('Hvilke roller tilsættes til brugeren'),
  );

  if (module_exists('os2dagsorden_token_login')) {
    $form['token_login'] = array(
      '#access' => (user_access('administer users')),
      '#type' => 'textfield',
      '#title' => t('Token login value:'),
      '#size' => 60,
      '#maxlength' => 128,
      '#description' => t('This is the token that can be used instead of the username/password combinations.'),
    );
  }

  if (!user_access('administer os2web')) {
    $form['user_roles']['administrator++']['#disabled'] = TRUE;
  }

  if (!(user_has_role(user_role_load_by_name("os2dagsorden limited")->rid))) {
    // DAGS-349 - enabling drag and drop for touch devices.
    _os2dagsorden_content_modify_render_drag_and_drop_controls($form, array($uid));
  }

  // Loading common user fields.
  if ($uid) {
    _os2dagsorden_conten_modify_load_user_fields($form, $uid);
  }

  $form['save_user'] = array(
    '#type' => 'submit',
    '#attributes' => array(
      'class' => array('form-save-submit'),
    ),
    '#value' => t('Save'),
    '#submit' => array('os2dagsorden_content_modify_user_submit'),
  );
  $form['cancel'] = array(
    '#type' => 'button',
    '#value' => t('Cancel'),
    '#prefix' => '&nbsp; &nbsp; &nbsp;',
    '#attributes' => array('class' => array('form-submit'), 'onClick' => 'history.go(-1); event.preventDefault();'),
	);

  $form[] = array(
  // <div class="node clearfix">.
    '#markup' => '</div>',
  );

  return $form;
}

/**
 * Fills forms with drag and drop controls for manipulating users committees.
 *
 * Member of/follows/follows+/follows++ and all committees.
 *
 * If uids is not null, the drag and drops controls will be prefilled with right
 * values.
 *
 * In case uids contains more than one id (batch edit), then only the committees
 * shared amount all users will be prefilled into the boxes.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $uids
 *   Array of uid.
 */
function _os2dagsorden_content_modify_render_drag_and_drop_controls(&$form, $uids) {
  $current_user = os2dagsorden_access_helper_get_user();

  $single_or_batch_class = (count($uids) === 1) ? 'single' : 'batch';

  $form[] = array(
    '#markup' => "<div class=\"select-committee {$single_or_batch_class}\">",
  );
  $is_admin = user_access('administer users');

  $form['member_of_div'] = array(
    '#access' => user_access('administer users'),
    '#type' => 'item',
    '#prefix' => '<label>' . t('Member of:') . '</label>' . (($is_admin) ? '<div class="note premeeting">' . t('Formøde publicering:') . '</div>' : '') . (($is_admin) ? '<div class="note committee-admin">' . t('Udvalgs admin:') . '</div>' : '' ),
    '#markup' => '',
  );
  $form['member_of_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'member_of_hidden',
    ),
  );
  $external_user = ((int) $current_user->field_user_external['und'][0]['value'] === 1);
  $user_can_change_follow_commitee = ((int)$current_user->uid === 1 || !(user_has_role(user_role_load_by_name("os2dagsorden limited")->rid) || $external_user));
  $form['follows_div'] = array(
    '#access' => $user_can_change_follow_commitee,
    '#type' => 'item',
    '#prefix' => '<label class="label_follows">' . t('Follows:') . '</label><div class="note subscribe">' . t('Notifikationer på mail:') . '</div>' . (($is_admin) ? '<div class="note premeeting">' . t('Formøde publicering:') . '</div>' : '' ) . (($is_admin) ? '<div class="note committee-admin">' . t('Udvalgs admin:') . '</div>' : '' ),
    '#markup' => '',
  );
  $form['follows_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'follows_hidden',
    ),
  );
  $form['follows_subscribed_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'follows_subscribed_hidden',
    ),
  );

  $form['follows_plus_div'] = array(
    '#access' => user_access('administer users'),
    '#type' => 'item',
    '#prefix' => '<label>' . t('Follows+:') . '</label>' . (($is_admin) ? '<div class="note premeeting">' . t('Formøde publicering:') . '</div>' : '') . (($is_admin) ? '<div class="note committee-admin">' . t('Udvalgs admin:') . '</div>' : '' ),
    '#markup' => '',

  );
  $form['follows_plus_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'follows_plus_hidden',
    ),
  );

  $form['follows_plus_plus_div'] = array(
    '#access' => user_access('administer users'),
    '#type' => 'item',
    '#prefix' => '<label>' . t('Follows++:') . '</label>' . (($is_admin) ? '<div class="note premeeting">' . t('Formøde publicering:') . '</div>' : '') . (($is_admin) ? '<div class="note committee-admin">' . t('Udvalgs admin:') . '</div>' : '' ),
    '#markup' => '',

  );
  $form['follows_plus_plus_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'follows_plus_plus_hidden',
    ),
  );
  $form['premeeting_committee_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'premeeting_committee_hidden',
    ),
  );
  $form['committee_admin_hidden'] = array(
    '#type' => 'hidden',
    '#attributes' => array(
      'id' => 'committee_admin_hidden',
    ),
  );
  $form[] = array(
    // <div class="select-committee">.
    '#markup' => '</div >',
  );

  $shared_all_user_committee = array();

  if ($uids) {
    $com_terms = os2dagsorden_settings_get_active_committees();
    $com_select = array();
    foreach ($com_terms as $com_term) {
      // Display term short name if possible.
      if ($short_name = field_get_items('taxonomy_term', $com_term, 'field_os2web_meetings_com_shname')) {
        $term_name = $short_name[0]['value'];
      }
      else {
        $term_name = $com_term->name;
      }
      $com_select[$com_term->tid] = $term_name;
    }
    $shared_premeeting_member_of = array();
    $shared_member_of = array();
    $shared_follows = array();
    $shared_follows_subscribed = array();
    $shared_follows_plus = array();
    $shared_follows_plus_plus = array();
    foreach ($uids as $uid) {
      $user = user_load($uid);

      // Premeeting member of.
      if (field_get_items('user', $user, 'field_user_premeeting_committee')) {
        $user_premeeting_member_of_field = field_get_items('user', $user, 'field_user_premeeting_committee');
        $user_premeeting_member_of_field = os2dagsorden_content_modify_asort($user_premeeting_member_of_field);
        foreach ($user_premeeting_member_of_field as $premeeting_member_of) {
          if (isset($com_select[$premeeting_member_of['tid']])) {
            $shared_premeeting_member_of[$premeeting_member_of['tid']]++;
            $shared_all_user_committee[$premeeting_member_of['tid']]++;
          }
        }
      }

      // Member of.
      if (field_get_items('user', $user, 'field_user_committee')) {
        $user_member_of_field = field_get_items('user', $user, 'field_user_committee');
        $user_member_of_field = os2dagsorden_content_modify_asort($user_member_of_field);
        foreach ($user_member_of_field as $member_of) {
          if (isset($com_select[$member_of['tid']])) {
            $shared_member_of[$member_of['tid']]++;
            $shared_all_user_committee[$member_of['tid']]++;
          }
        }
      }
      // Follows.
      if (field_get_items('user', $user, 'field_follows_committee')) {
        $user_follows_field = field_get_items('user', $user, 'field_follows_committee');
        $user_follows_field = os2dagsorden_content_modify_asort($user_follows_field);
        foreach ($user_follows_field as $follows) {
          if (isset($com_select[$follows['tid']])) {
            $shared_follows[$follows['tid']]++;
            $shared_all_user_committee[$follows['tid']]++;
          }
        }
      }

      // Follows subscribed, only for single user edit.
      if (field_get_items('user', $user, 'field_follows_committee_subsc')) {
        $user_follows_subscribed_field = field_get_items('user', $user, 'field_follows_committee_subsc');
        foreach ($user_follows_subscribed_field as $follows_subs) {
          if (isset($com_select[$follows_subs['tid']])) {
            $shared_follows_subscribed[$follows_subs['tid']]++;
          }
        }
      }
      // Follows plus.
      if (field_get_items('user', $user, 'field_follows_committee_plus')) {
        $user_follows_plus_field = field_get_items('user', $user, 'field_follows_committee_plus');
        $user_follows_plus_field = os2dagsorden_content_modify_asort($user_follows_plus_field);

        foreach ($user_follows_plus_field as $follows_plus) {
          if (isset($com_select[$follows_plus['tid']])) {
            $shared_follows_plus[$follows_plus['tid']]++;
            $shared_all_user_committee[$follows_plus['tid']]++;
          }
        }
      }

      // Follows plus plus.
      if (field_get_items('user', $user, 'field_follows_committee_pp')) {
        $user_follows_plus_plus_field = field_get_items('user', $user, 'field_follows_committee_pp');
        $user_follows_plus_plus_field = os2dagsorden_content_modify_asort($user_follows_plus_plus_field);
        foreach ($user_follows_plus_plus_field as $follows_plus_plus) {
          if (isset($com_select[$follows_plus_plus['tid']])) {
            $shared_follows_plus_plus[$follows_plus_plus['tid']]++;
            $shared_all_user_committee[$follows_plus_plus['tid']]++;
          }
        }
      }
    }
    // Printing premeeting member of.
    /*$content = "<ul id='premeeting_member_of' class='droptrue'>";
    foreach ($shared_premeeting_member_of as $tid => $count) {
      if ($count == count($uids)) {
        $form['premeeting_member_of_hidden']['#value'] .= $tid . ',';
        $content .= "<li id='{$tid}' class='ui-state-default'><span>{$com_select[$tid]}</span><input id='premeeting_checkbox_$tid' type='checkbox' class='checkbox premeeting'></li>";
      }
    }
    $content .= '</ul>';
    $form['premeeting_member_of_div']['#markup'] = $content;
*/
    // Printing member of.
    $content = "<ul id='member_of' class='droptrue'>";
    foreach ($shared_member_of as $tid => $count) {
      if ($count === count($uids)) {
        $form['member_of_hidden']['#value'] .= $tid . ',';
        if (count($uids) === $shared_premeeting_member_of[$tid]) {
          $form['premeeting_committee_hidden']['#value'] .= $tid . ',';
          $premeeting_check_id = $tid;
        }
        $checked = '';
        if ($premeeting_check_id === $tid) {
          $checked = "checked='checked'";
        }
        $is_commitee_admin = os2dagsorden_access_helper_is_user_committee_admin($tid, $user->uid);
        $is_commitee_admin_checked = $is_commitee_admin ? "checked='checked'" : '';
        if ($is_commitee_admin) {
          $form['committee_admin_hidden']['#value'] .= $tid . ',';
        }
        $content .= "<li id='{$tid}' class='ui-state-default'><span>{$com_select[$tid]}</span>";
        if ($is_admin) {
          $content .= " <span class='checkbox premeeting'><input id='premeeting_checkbox_$tid' type='checkbox' class='' {$checked}></span>";
        }
         if ($is_admin) {
          $content .= " <span class='checkbox committee-admin'><input id='committee_admin_checkbox_$tid' type='checkbox' class='' {$is_commitee_admin_checked}></span>";
        }
                $content .= '</li>';
      }
    }
    $content .= '</ul>';
    $form['member_of_div']['#markup'] = $content;

    // Printing follows.
    $content = "<ul id='follows' class='droptrue'>";
    foreach ($shared_follows as $tid => $count) {
      if ($count === count($uids)) {
        $form['follows_hidden']['#value'] .= $tid . ',';

        // Setting correct follows subscribed marker.
        $subscribed_class = 'can-subscribe';
        if (count($uids) === $shared_follows_subscribed[$tid]) {
          $subscribed_class .= ' subscribed';
          $form['follows_subscribed_hidden']['#value'] .= $tid . ',';
          $check_id = $tid;
        }
        if (count($uids) === $shared_premeeting_member_of[$tid]) {
          $form['premeeting_committee_hidden']['#value'] .= $tid . ',';
          $premeeting_check_id = $tid;
        }
        $checked = '';
        if ($premeeting_check_id === $tid) {
          $checked = "checked='checked'";
        }
        $is_commitee_admin = os2dagsorden_access_helper_is_user_committee_admin($tid, $user->uid);
        $is_commitee_admin_checked = $is_commitee_admin ? "checked='checked'" : '';
        if ($is_commitee_admin) {
          $form['committee_admin_hidden']['#value'] .= $tid . ',';
        }
        if ($check_id === $tid) {
          $content .= "<li id='{$tid}' class='ui-state-default'><span>{$com_select[$tid]}</span><span class='checkbox {$subscribed_class}'><input id='follows_checkbox_$tid' type='checkbox' class='checkbox_follows' checked='checked'></span>";
          if ($is_admin) {
            $content .= "<span class='checkbox premeeting'><input id='premeeting_checkbox_$tid' type='checkbox' class='' {$checked}></span>";
          }
          if ($is_admin) {
            $content .= " <span class='checkbox committee-admin'><input id='committee_admin_checkbox_$tid' type='checkbox' class='' {$is_commitee_admin_checked}></span>";
          }
          $content .= '</li>';
        }
        else {
          $content .= "<li id='{$tid}' class='ui-state-default'><span>{$com_select[$tid]}</span><span class='checkbox {$subscribed_class}'><input id='follows_checkbox_$tid' type='checkbox' class='checkbox_follows'></span>";
          if ($is_admin) {
            $content .= "<span class='checkbox premeeting'><input id='premeeting_checkbox_$tid' type='checkbox' class='' {$checked}></span>";
          }
          if ($is_admin) {
            $content .= " <span class='checkbox committee-admin'><input id='committee_admin_checkbox_$tid' type='checkbox' class='' {$is_commitee_admin_checked}></span>";
          }
          $content .= '</li>';
        }
      }
    }
    $content .= '</ul>';
    $form['follows_div']['#markup'] = $content;

    // Printing follows plus.
    $content = "<ul id='follows_plus' class='droptrue'>";
    foreach ($shared_follows_plus as $tid => $count) {
      if ($count === count($uids)) {
        $form['follows_plus_hidden']['#value'] .= $tid . ',';
        if (count($uids) === $shared_premeeting_member_of[$tid]) {
          $form['premeeting_committee_hidden']['#value'] .= $tid . ',';
          $premeeting_check_id = $tid;
        }
        $checked = '';
        if ($premeeting_check_id === $tid) {
          $checked = "checked='checked'";
        }
        $content .= "<li id='{$tid}' class='ui-state-default'><span>{$com_select[$tid]}</span>";
       $is_commitee_admin = os2dagsorden_access_helper_is_user_committee_admin($tid, $user->uid);
       $is_commitee_admin_checked = $is_commitee_admin ? "checked='checked'" : '';
        if ($is_commitee_admin) {
          $form['committee_admin_hidden']['#value'] .= $tid . ',';
        }
        if ($is_admin) {
          $content .= "<span class='checkbox premeeting'><input id='premeeting_checkbox_$tid' type='checkbox' class='' {$checked}></span>";
        }
        if ($is_admin) {
          $content .= " <span class='checkbox committee-admin'><input id='committee_admin_checkbox_$tid' type='checkbox' class='' {$is_commitee_admin_checked}></span>";
        }
        $content .= '</li>';
      }
    }
    $content .= '</ul>';
    $form['follows_plus_div']['#markup'] = $content;

    // Printing follows plus plus.
    $content = "<ul id='follows_plus_plus' class='droptrue'>";
    foreach ($shared_follows_plus_plus as $tid => $count) {
      if ($count === count($uids)) {
        $form['follows_plus_plus_hidden']['#value'] .= $tid . ',';
        if (count($uids) === $shared_premeeting_member_of[$tid]) {
          $form['premeeting_committee_hidden']['#value'] .= $tid . ',';
          $premeeting_check_id = $tid;
        }
        $checked = '';
        $is_commitee_admin = os2dagsorden_access_helper_is_user_committee_admin($tid, $user->uid);
        $is_commitee_admin_checked = $is_commitee_admin ? "checked='checked'" : '';
        if ($is_commitee_admin) {
          $form['committee_admin_hidden']['#value'] .= $tid . ',';
        }
        if ($premeeting_check_id === $tid) {
          $checked = "checked='checked'";
        }
        $content .= "<li id='{$tid}' class='ui-state-default'><span>{$com_select[$tid]}</span>";
        if ($is_admin) {
          $content .= "<span class='checkbox premeeting'><input id='premeeting_checkbox_$tid' type='checkbox' class='' {$checked}></span>";
        }
        if ($is_admin) {
          $content .= " <span class='checkbox committee-admin'><input id='committee_admin_checkbox_$tid' type='checkbox' class='' {$is_commitee_admin_checked}></span>";
        }
        $content .= '</li>';
      }
    }
    $content .= '</ul>';
    $form['follows_plus_plus_div']['#markup'] = $content;

    if (!$external_user) {
      // Printing arrows reverse.
      $form[]['#markup'] =
        '<div class="arrows-reverse">
          <img src = "' . base_path() . drupal_get_path('theme', 'os2dagsorden_classic_theme') . '/img/img_arrows_reverse.png">
        </div>';
    }
    // Printing available committees.
    $com_list = "<ul id='available_committee' class='droptrue'>";
    asort($com_select);
    foreach ($com_select as $tid => $com_term) {
      // If the committee actually available?
      if ($shared_all_user_committee[$tid] < count($uids)) {
        $com_list .= "<li id='{$tid}' class='ui-state-default'><span>{$com_term}</span><span class='checkbox'><input id='follows_checkbox_$tid'  type='checkbox' class='checkbox'></span>";
        if ($is_admin) {
          $com_list .= "<span class='checkbox'><input id='premeeting_checkbox_$tid' type='checkbox' class='' ></span>";
        }
        if ($is_admin) {
          $com_list .= " <span class='checkbox'><input id='committee_admin_checkbox_$tid' type='checkbox' class=''></span>";
        }

        $com_list .= '</li>';
      }
    }
    $com_list .= "</ul>";
    $form['available_committee'] = array(
      '#access' => $user_can_change_follow_commitee,
      '#type' => 'item',
      '#prefix' => "<div class='available_committee'><label>" . t('Available committee:') . "</label>",
      '#markup' => $com_list,
      '#suffix' => '</div>',
    );
    $form[] = array(
      '#markup' => '<div style="clear:both"></div>',
    );
  }
}

/**
 * Prefills the form with the user information.
 *
 * Loads everything except the committees preferences.
 *
 * @param mixed $form
 *   The form.
 * @param int $uid
 *   Single uid.
 */
function _os2dagsorden_conten_modify_load_user_fields(&$form, $uid) {
  // Saving for usage in the submit function.
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $uid,
  );
  $user = user_load($uid);

  // Filling the fields.
  if (field_get_items('user', $user, 'field_user_full_name')) {
    $full_name_field = array_pop(field_get_items('user', $user, 'field_user_full_name'));
    $form['full_name']['#default_value'] = $full_name_field['value'];
  }
  if (field_get_items('user', $user, 'field_user_organization')) {
    $organization_field = array_pop(field_get_items('user', $user, 'field_user_organization'));
    $form['organization']['#default_value'] = $organization_field['value'];
  }

  if (field_get_items('user', $user, 'field_user_can_send_closed_bp')) {
    $user_can_send_closed_bp = array_pop(field_get_items('user', $user, 'field_user_can_send_closed_bp'));
    $form['user_can_send_closed_bp']['#default_value'] = $user_can_send_closed_bp['value'];
  }

  if (field_get_items('user', $user, 'field_user_com_forbid_categ')) {
    $forbidden_cats_field = field_get_items('user', $user, 'field_user_com_forbid_categ');
    $forbidden_cats_selection = array();

    foreach ($forbidden_cats_field as $forbidden_cat) {
      $forbidden_cats_selection[] = $forbidden_cat['tid'];
    }
    $form['forbidden_cat']['#default_value'] = $forbidden_cats_selection;
    $form['allowed_cat']['#default_value'] = array_diff(array_keys($form['calendar_pref']['#options']), $forbidden_cats_selection);

    $options = $form['calendar_pref']['#options'];
    foreach ($forbidden_cats_selection as $forbidden_cat) {
      unset($options[$forbidden_cat]);
    }
    $form['calendar_pref']['#options'] = $options;
  }
  else {
    $form['allowed_cat']['#default_value'] = array_keys($form['calendar_pref']['#options']);
  }

  if (field_get_items('user', $user, 'field_user_com_categ')) {
    $calendar_prefs_field = field_get_items('user', $user, 'field_user_com_categ');
    $selection = array();
    foreach ($calendar_prefs_field as $cal_pref) {
      $selection[] = $cal_pref['tid'];
    }

    $form['calendar_pref']['#default_value'] = $selection;
  }

  if (field_get_items('user', $user, 'field_user_login_token')) {
    $token_login_field = array_pop(field_get_items('user', $user, 'field_user_login_token'));
    $form['token_login']['#default_value'] = $token_login_field['value'];
  }
  if (field_get_items('user', $user, 'field_user_external')) {
    $user_external_field = array_pop(field_get_items('user', $user, 'field_user_external'));
    $form['user_external']['#default_value'] = $user_external_field['value'];
  }
  $form['user_roles']['#default_value'] = $user->roles;
}

/**
 * Submit for user edit form. Updates user profile.
 *
 * @param mixed $form
 *   The form.
 * @param mixed $form_state
 *   The state of the form.
 */
function os2dagsorden_content_modify_user_submit($form, $form_state) {
  $input = $form_state['input'];

  // Input extraction.
  extract($input);

  $follows = explode(',', $follows_hidden);
  $follows_subscribed = explode(',', $follows_subscribed_hidden);
  $premeeting_member_of = explode(',', $premeeting_committee_hidden);
  $member_of = explode(',', $member_of_hidden);
  $follows_plus = explode(',', $follows_plus_hidden);
  $follows_plus_plus = explode(',', $follows_plus_plus_hidden);
  $committee_admin = explode(',', $committee_admin_hidden);
  $committee_admin= array_filter($committee_admin, function($a) { return ($a != 0); });

  // If the forbidden categories are not preset and categories mode is allowed,
  // fill the variable artificially.
  if (!isset($forbidden_cat) && variable_get('os2dagsorden_committee_forbidden_allowed_categories_mode', 'forbidden') === 'allowed') {
    $cat_voc = taxonomy_vocabulary_machine_name_load('os2web_meetings_tax_committee_category');
    $cat_terms = taxonomy_get_tree($cat_voc->vid);
    $cat_all = array();
    foreach ($cat_terms as $cat_term) {
      $cat_all[] = $cat_term->tid;
    }

    $forbidden_cat = array_diff($cat_all, $allowed_cat);
  }

  $user = user_load($form_state['values']['uid']);
  $user->field_user_full_name['und'][0]['value'] = $full_name;
  $user->field_user_organization['und'][0]['value'] = $organization;
  $user->field_user_can_send_closed_bp['und'][0]['value'] = $user_can_send_closed_bp;
  $user->field_user_external['und'][0]['value'] = $user_external;
  // Unsetting follows committees.
  $user->field_follows_committee['und'] = array();
  if ($follows) {
    foreach ($follows as $tid) {
      if (!empty($tid)) {
        $user->field_follows_committee['und'][]['tid'] = $tid;
      }
    }
  }

  // Unsetting follows subscribed committees only on desktop devices.
  $user->field_follows_committee_subsc['und'] = array();
  if ($follows_subscribed) {
    foreach ($follows_subscribed as $tid) {
      if (!empty($tid)) {
        $user->field_follows_committee_subsc['und'][]['tid'] = $tid;
      }
    }
  }

  // Unsetting calendar preferences.
  $user->field_user_com_categ['und'] = array();
  if (isset($calendar_pref)) {
    foreach ($calendar_pref as $cal_pref) {
      if (isset($forbidden_cat)) {
        if (!in_array($cal_pref, $forbidden_cat)) {
          $user->field_user_com_categ['und'][]['tid'] = $cal_pref;
        }
      }
      else {
        $user->field_user_com_categ['und'][]['tid'] = $cal_pref;
      }
    }
  }

  // Only for the full form submit.
  if (user_access('administer users')) {
    // Unsetting calendar preferences.
    $user->field_user_com_forbid_categ['und'] = array();
    foreach ($forbidden_cat as $f_cat) {
      $user->field_user_com_forbid_categ['und'][]['tid'] = $f_cat;
    }
    // Unsetting premeeting committees.
    $user->field_user_premeeting_committee['und'] = array();
    if ($premeeting_member_of) {
      foreach ($premeeting_member_of as $tid) {
        if (!empty($tid)) {
          $user->field_user_premeeting_committee['und'][]['tid'] = $tid;
        }
      }
    }
    $committee_user_is_admin =  os2dagsorden_access_helper_get_all_committee_user_is_admin($user->uid);
    $committee_to_add = array_diff($committee_admin, $committee_user_is_admin);
    $committee_to_delete =  array_diff($committee_user_is_admin, $committee_admin);
    if (!empty($committee_to_add)) {
      foreach ($committee_to_add as $tid) {
        $committee = taxonomy_term_load($tid);
        $committee->field_os2web_meetings_com_admin['und'][]['uid'] = $user->uid;
        taxonomy_term_save($committee);
      }
    }
    if (!empty($committee_to_delete)) {
      foreach ($committee_to_delete as $key => $tid) {
        $committee = taxonomy_term_load($tid);
        foreach ($committee->field_os2web_meetings_com_admin['und'] as $key => $term) {
          if ($term['uid'] == $user->uid) {
            unset($committee->field_os2web_meetings_com_admin['und'][$key]);
            taxonomy_term_save($committee);
            break;
          }
        }
      }
    }
    // Unsetting committees.
    $user->field_user_committee['und'] = array();
    if ($member_of) {
      foreach ($member_of as $tid) {
        if (!empty($tid)) {
          $user->field_user_committee['und'][]['tid'] = $tid;
        }
      }
    }

    // Unsetting follows plus committees.
    $user->field_follows_committee_plus['und'] = array();
    if ($follows_plus) {
      foreach ($follows_plus as $tid) {
        if (!empty($tid)) {
          $user->field_follows_committee_plus['und'][]['tid'] = $tid;
        }
      }
    }

    // Unsetting follows ++ committees.
    $user->field_follows_committee_pp['und'] = array();
    if ($follows_plus_plus) {
      foreach ($follows_plus_plus as $tid) {
        if (!empty($tid)) {
          $user->field_follows_committee_pp['und'][]['tid'] = $tid;
        }
      }
    }

    // User to token field.
    $user->field_user_login_token['und'][0]['value'] = $token_login;

    // os2dagsorden limited role.
    $user_roles_saved = $user->roles;

    // os2dagsorden admin role.
    if (user_access('administer os2web')) {
      if (!$user_roles['administrator++']) {
        if (($key = array_search('administrator++', $user_roles_saved)) !== FALSE) {
          unset($user_roles_saved[$key]);
        }
      }
      else {
        if ((array_search('administrator++', $user_roles_saved)) === FALSE) {
          $rid = user_role_load_by_name('administrator++')->rid;
          $user_roles_saved[$rid] = 'administrator++';
        }
      }
    }
    if (!$user_roles['os2dagsorden limited']) {
      if (($key = array_search('os2dagsorden limited', $user_roles_saved)) !== FALSE) {
        unset($user_roles_saved[$key]);
      }
    }
    else {
      if ((array_search('os2dagsorden limited', $user_roles_saved)) === FALSE) {
        $rid = user_role_load_by_name('os2dagsorden limited')->rid;
        $user_roles_saved[$rid] = 'os2dagsorden limited';
      }
    }

    if (!$user_roles['administrator+']) {
      if (($key = array_search('administrator+', $user_roles_saved)) !== FALSE) {
        unset($user_roles_saved[$key]);
      }
    }
    else {
      if ((array_search('administrator+', $user_roles_saved)) === FALSE) {
        $rid = user_role_load_by_name('administrator+')->rid;
        $user_roles_saved[$rid] = 'administrator+';
      }
    }
    $user->roles = $user_roles_saved;
  }

  user_save($user);
  drupal_set_message(t('Dine ændringer er blevet gemt'));
}

/**
 * Sorting terms alphabetically based on field_os2web_meetings_com_shname.
 *
 * @param mixed $term_list
 *   The list of terms.
 *
 * @return mixed
 *   Sorted array of terms.
 */
function os2dagsorden_content_modify_asort($term_list = array()) {
  $new_array = array();
  $return_array = array();
  if ($term_list) {
    foreach ($term_list as $tid) {
      if ($term = taxonomy_term_load($tid['tid'])) {
        if ($short_name = field_get_items('taxonomy_term', $term, 'field_os2web_meetings_com_shname')) {
          $term_name = $short_name[0]['value'];
        }
        else {
          $term_name = $term->name;
        }
        $new_array[$tid['tid']] = $term_name;
      }
    }
    asort($new_array);

    foreach ($new_array as $tid => $name) {
      $return_array[] = array('tid' => $tid);
    }
  }
  return $return_array;
}
