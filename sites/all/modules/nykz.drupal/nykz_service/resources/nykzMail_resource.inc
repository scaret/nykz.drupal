<?php

function _nykzMail_resource_index(){
  global $user;
  $username = $user->name;
  $ret = nykz_mail_index();
  return $ret;

}