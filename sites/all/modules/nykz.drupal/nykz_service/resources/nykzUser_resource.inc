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

  function _nykzUser_resource_retrieve($username){
    return nykz_user_get_uid_by_name($username);
  }