

/******************************函数声明*************************/
int binary_search(float search_table[],int length,float key);

/*
**************************************************************
*@ funciton			: int binary_search(int search_table[],int length,int key)  
*@ brief			: 折半查找 
*@ param 			: search_table：查找的表；length：表的数据的长度；key需要查找的值
*@ return			: 失败返回-1；成功返回查找的值的下标
***********************************************************************
*/
// 非递归折半  
int binary_search(float search_table[],int length,float key) 
{  
    // 最低位置索引low、最高位置索引high、中间位置索引mid  
  
    // 中间位置的可能情况  
    // length为奇数时，mid 为正中间位置 mid的左侧和右侧用于同样数目的元素  
    // length为偶数时，mid为正中间往左的那一个元素   正中间为小数，正中间往左的那一个位置才是(Low+High)/2  
  
  
    // low与high的关系  
    // 正常情况下low<high  
    // low==high时，仅剩下最后一个需要判断的元素，此元素可能与key相同，也可能不同  
    // low>high 未找到与key相同的元素  
    int low=0;  
    int high=length-1;  
    int mid=(low+high)/2;  
  
    while(low<=high)
	{  
  
        mid=(low+high)/2;  
  
        //找到与key相等的一个元素位置  
        if((search_table[mid]==key)||((key>search_table[mid])&&(key<search_table[mid+1])))  
            return mid;
  
        if (search_table[mid]>key)  
            high=mid-1;  
        else  
            low=mid+1;  
    }  
  
    return -1;  
  
} 


 

