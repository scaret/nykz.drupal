<?php
/**
 * 列出南洋客栈板面列表
 */
function _nykzBoard_resource_index(){
  $boards = nykz_board_load_multiple();
  foreach($boards as $board){
    $ret[] = array(
      'board_catagory_name' => $board['board_catagory_name'],
      'board_chinesename' => $board['board_chinesename'],
      'board_name' => $board['board_name'],
      );
  }
  return $ret;
}