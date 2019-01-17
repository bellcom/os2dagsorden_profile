<?php
/**
 * @file
 * os2dagsorden_rules_and_permissions.rules_defaults.inc
 */

/**
 * Implements hook_default_rules_configuration().
 */
function os2dagsorden_rules_and_permissions_default_rules_configuration() {
  $items = array();
  $items['rules_disable_direct_content_view_for_non_admins'] = entity_import('rules_config', '{ "rules_disable_direct_content_view_for_non_admins" : {
      "LABEL" : "Disable direct content view for non-admins",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "rules" ],
      "ON" : { "node_view" : [] },
      "IF" : [
        { "node_is_of_type" : {
            "node" : [ "node" ],
            "type" : { "value" : {
                "bullet_point" : "bullet_point",
                "bullet_point_attachment" : "bullet_point_attachment",
                "speaker_paper" : "speaker_paper"
              }
            }
          }
        }
      ],
      "DO" : [ { "redirect" : { "url" : "\\u003Cfront\\u003E" } } ]
    }
  }');
  $items['rules_os2web_set_committee_from_simplesaml'] = entity_import('rules_config', '{ "rules_os2web_set_committee_from_simplesaml" : {
      "LABEL" : "OS2web set committee from simplesaml",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "REQUIRES" : [ "php", "rules", "simplesamlphp_auth" ],
      "ON" : { "simplesamlphp_auth_rules_event_login" : [] },
      "DO" : [
        { "php_eval" : { "code" : "$user = os2dagsorden_access_helper_get_user();\\r\\n$user_committees = array();\\r\\n\\r\\n$field_user_committee = field_get_items(\\u0027user\\u0027, $user, \\u0027field_user_committee\\u0027);\\r\\nif (!empty($field_user_committee)) {\\r\\n  foreach ($field_user_committee as $user_committee) {\\r\\n    $user_committees[$user_committee[\\u0027tid\\u0027]] = $user_committee[\\u0027tid\\u0027];\\r\\n  }\\r\\n}\\r\\n\\r\\n$groups = simplesamlphp_auth_get_attributes(\\u0027http:\\/\\/schemas.xmlsoap.org\\/claims\\/Group\\u0027);\\r\\nforeach ($groups as $group) {\\r\\n  if (preg_match(\\u0027\\/.*_OS2Dagsorden_(.*)\\/i\\u0027, $group, $matches)) {\\r\\n    $committee_name = $matches[1];\\r\\n\\r\\n    $committee = taxonomy_get_term_by_name($committee_name, \\u0027os2web_meetings_tax_committee\\u0027);\\r\\n    if ($committee) {\\r\\n      $committee = reset($committee);\\r\\n      $updated_user_committees[$committee-\\u003Etid] = $committee-\\u003Etid;\\r\\n    }\\r\\n  }\\r\\n}\\r\\n\\r\\nsort($user_committees);\\r\\nsort($updated_user_committees);\\r\\n\\r\\nif ($user_committees != $updated_user_committees) {\\r\\n  $user-\\u003Efield_user_committee[\\u0027und\\u0027] = array();\\r\\n\\r\\n  foreach($updated_user_committees as $tid) {\\r\\n    $user-\\u003Efield_user_committee[\\u0027und\\u0027][][\\u0027tid\\u0027] = $tid;\\r\\n  }\\r\\n\\r\\n  user_save($user);\\r\\n}" } }
      ]
    }
  }');
  return $items;
}
