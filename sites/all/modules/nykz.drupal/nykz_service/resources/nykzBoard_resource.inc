<?php
/**
 * 列出南洋客栈板面列表
 */
function _nykzBoard_resource_index(){
  global $user;
  if(!isset($user) || !$user->uid){
    $username = "guest";
  }
  else{
    $username = $user->name;
  }
  $boards = nykz_board_load_multiple(array(), $username);
  foreach($boards as $board){
    $ret[] = array(
      'catagory_name' => $board['board_catagory_name'],
      'chinesename' => $board['board_chinesename'],
      'name' => $board['board_name'],
      'iskboard' => $board['board_iskboard'],
      );
  }
  return $ret;
}

/**
 * 列出南洋客栈板面详细信息
 */
function _nykzBoard_resource_retrieve($board_name){
  global $user;
  if(!isset($user) || !$user->uid){
    $username = "guest";
  }
  else{
    $username = $user->name;
  }
  $boards = nykz_board_load_multiple(array($board_name), $username);
  if(!count($boards)){
    return services_error( t("Board named $board_name not found."), 404);
  }
  $board = reset($boards);
  return array(
      'catagory_name' => $board['board_catagory_name'],
      'chinesename' => $board['board_chinesename'],
      'name' => $board['board_name'],
      'iskboard' => $board['board_iskboard'],
      'bm' => $board['board_bm'],
      'catagory' => $board['board_catagory'],
    );
}
