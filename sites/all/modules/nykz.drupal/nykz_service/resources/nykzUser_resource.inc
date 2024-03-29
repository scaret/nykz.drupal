<?php
  function _nykzUser_resource_create($username,$pw){
    if( nykz_user_get_uid_by_name($username) ){
      throw new Exception("The user already exists.");
    }
    elseif( !nykz_user_validate($username,$pw)){
      throw new Exception("Invalid username or password.");
    }
    else{
      $uid = nykz_user_create($username,$pw);
      return $uid;
    }
  }

  function _nykzUser_resource_index(){
    global $user;
    if($user->uid){
      return _nykzUser_resource_retrieve($user->name);
    }
  }

  function _nykzUser_resource_retrieve($username){
    $nykzuser = nykz_user_load($username);
    if(!$nykzuser){
      throw new Exception("User doesn't exist.");
    }
    else{
      return array(
        'userid' => $nykzuser['userid'],
        'username' => $nykzuser['username'],
        'gender' => $nykzuser['gender'],
        'numlogins' => $nykzuser['numlogins'],
        'nummails' => $nykzuser['nummails'],
        'birthmonth' => $nykzuser['birthmonth'],
        'birthday' => $nykzuser['birthday'],
        'firstlogin' => $nykzuser['firstlogin'],
        'inbank' => $nykzuser['inbank'],
        'userlevel' => $nykzuser['userlevel'],
        'userperm' => $nykzuser['userperm'],
      );
    }
  }

  function _nykzUser_resource_login($username,$pw){
    if(!nykz_user_validate($username,$pw)){
      return services_error( t("Invalid username or password."), 403);
    }
    $uid = nykz_user_get_uid_by_name($username);
    if(!$uid){
      _nykzUser_resource_create($username,$pw);
    }
    module_load_include('inc', 'oauth_common', 'oauth_common');
    $ret =  nykz_user_change_key($username);
    $info = array('name' => $username);

    // login the user in drupal
    global $user;
    $account = user_load_by_name($username);

    $form_submit = array('uid' => $account->uid);
    user_login_submit(array(), $form_submit);

    return array(
      'uid' => $ret->uid,
      'name' => $username,
      'key' => $ret->key,
      'secret' => $ret->secret,
      );
  }
