<?php

/**
 * Nemlogin login callback.
 *
 * Initiates IDP call or stores the values.
 *
 * @throws \Exception
 */
function os2web_nemlogin_login_callback() {
  // No token found, initiate login.
  if (empty($_REQUEST['token'])) {
    $forward_url = $GLOBALS['base_url'] . request_uri();

    $url = url(variable_get('os2web_nemlogin_idp_base_url') . OS2WEB_NEMLOGIN_BEL_OS2_LOGIN_PATH, [
      'query' => [
        'mnemo' => variable_get('os2web_nemlogin_mnemo'),
        'forward' => $forward_url,
      ],
    ]);

    drupal_goto($url);
  }

  // Token is obtained, use it for getting values.
  $token = $_REQUEST['token'];
  $mnemo = $_REQUEST['mnemo'];

  $uid = NULL;

  $data = "token=$token&mnemo=$mnemo";

  $responseRaw = drupal_http_request(variable_get('os2web_nemlogin_idp_base_url') . OS2WEB_NEMLOGIN_BEL_OS2_REST_PATH, [
    'method' => 'POST',
    'data' => $data,
    'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
  ]);

  $response = json_decode($responseRaw->data) ;

  if ($response) {
    $uidField = variable_get('os2web_nemlogin_uid_field', 'pid');
    if (isset($response->{$uidField})) {
      $uidRaw = $response->{$uidField};
      if ($uidRaw) {
        $uid = utf8_decode($uidRaw);
        if ($uidField == 'cpr') {
          $uid = hash('sha256',drupal_get_hash_salt() . $uid);
        }

        $_SESSION['nemlogin_uid'] = $uid;
      }
    }
  }

  if (!$uid) {
    watchdog('os2web_nemlogin', "Could not fetch UID field. Response is empty");
  }

  // Saving token into a sessions.
  os2web_nemlogin_add_token($token, session_id());

  // Logout on IDP.
  $getParams = http_build_query([
    'RelayState' => ($_SESSION['os2intra_simplesaml_user_login_link']) ? $GLOBALS['base_url'] . '/' . $_SESSION['os2intra_simplesaml_user_login_link'] : $GLOBALS['base_url'],
  ]);

  $logoutUrl = variable_get('os2web_nemlogin_idp_base_url') . OS2WEB_NEMLOGIN_BEL_OS2_LOGOUT_PATH . '?' . $getParams;
  drupal_goto($logoutUrl);
}

/**
 * Destroys the local session by token in POST.
 */
function os2web_nemlogin_session_destroy() {
  $token = $_POST['token'];

  if ($token) {
    // Find session id.
    $sid = os2web_nemlogin_get_session_by_token($token);

    if ($sid) {
      $deleted = _drupal_session_destroy($sid);

      // Finally removing token.
      if ($deleted) {
        os2web_nemlogin_remove_token($token);
      }
    }
  }
}

/**
 * Maps token and session id
 *
 * @param string $token
 *   Token.
 * @param string  $sid
 *   Session ID.
 *
 * @throws \Exception
 */
function os2web_nemlogin_add_token($token, $sid) {
  db_insert('os2web_nemlogin_session_tokens')->fields([
    'token' => $token,
    'sid' => $sid,
  ])->execute();
}

/**
 * Returns sessions ID by token.
 *
 * @param string $token
 *   Token.
 *
 * @return mixed
 *   Session ID.
 */
function os2web_nemlogin_get_session_by_token($token) {
  $sid = db_select('os2web_nemlogin_session_tokens', 'o')
    ->fields('o', ['sid'])
    ->condition('token', $token,'=')
    ->execute()
    ->fetchCol();

  return $sid;
}

/**
 * Removed mapping between token and session ID.
 *
 * @param $token
 *   String token.
 */
function os2web_nemlogin_remove_token($token) {
  db_delete('os2web_nemlogin_session_tokens')
    ->condition('token', $token)
    ->execute();
}

function os2web_nemlogin_select_login() {
  return (
  '<a href="/nemlogin/login">Nemid</a><br/><a href="/saml_login">SimpleSAML</a>'
  );
}

