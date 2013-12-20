<?php
/**
 * 列出南洋客栈帖子列表
 */
function _nykzPost_resource_index($boardName, $start, $count){
  $posts = nykz_post_index($boardName,$start, $count);
  return $posts;
}

/**
 * 获取某个帖子
 */
function _nykzPost_resource_retrieve($boardName, $filename, $format = 'novt'){
  $the_post = nykz_post_load($boardName, $filename, $format);
  return $the_post;
}
