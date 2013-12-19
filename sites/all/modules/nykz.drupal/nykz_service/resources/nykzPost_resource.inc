<?php
/**
 * 列出南洋客栈帖子列表
 */
function _nykzPost_resource_index(){
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

/**
 * 获取某个帖子
 */
function _nykzPost_resource_retrieve($boardName, $filename, $format = 'novt'){

  $content = nykz_post_load($boardName, $filename, $format);
  return $content;
}
