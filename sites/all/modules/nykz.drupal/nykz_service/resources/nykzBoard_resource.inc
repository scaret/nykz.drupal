<?php
/**
 * 列出南洋客栈板面列表
 */
function _nykzBoard_resource_index(){
  $boards = nykz_board_load_multiple();
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