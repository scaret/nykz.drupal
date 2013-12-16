<?php
  function _nykzUser_resource_create($username,$pw){
    return array($username,$pw);
  }

  function _nykzUser_resource_retrieve($username){
    return $username;
  }