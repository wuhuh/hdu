#include<stdio.h>
#include<vector>
using namespace std;
#define max 50050
int a[max];//原始数据
vector<int> line[max];
int fa[max],d[max],size[max],son[max],top[max];
int num[max],rnum[max];//rnum[i]是i在原序列的序号，num[i]是i在重链的序号
int cnt;
int n;
long long root[8*max];
long long lazy[8*max];
//先进行剖分
void dfs1(int u,int v){
    fa[v]=u;
    d[v]=d[u]+1;
    size[v]=1;
    son[v]=0;
    for(int i=0;i<line[v].size();i++)
        if(line[v][i]!=u) {
            dfs1(v,line[v][i]);
            size[v]+=size[line[v][i]];
            if(size[line[v][i]]>size[son[v]]) son[v]=line[v][i];
        }
}
void dfs2(int u,int topp){
    num[u]=++cnt;
    rnum[cnt]=u;
    top[u]=topp;
    if(son[u]){
        dfs2(son[u],topp);
        for(int i=0;i<line[u].size();i++){
            if(line[u][i]!=son[u]&&line[u][i]!=fa[u]) dfs2(line[u][i],line[u][i]);
        }
    }
}
//接下来构建线段树
void pushup(int x){
    root[x]=root[x*2]+root[x*2+1];
}
void pushdown(int x,int len){
    if(lazy[x]){
        root[x*2]+=lazy[x]*(len-len/2);
        root[x*2+1]+=lazy[x]*(len/2);
        lazy[x*2]+=lazy[x];
        lazy[x*2+1]+=lazy[x];
        lazy[x]=0;
    }
}
void bulidtree(int l,int r,int now){
    lazy[now]=0;
    if(l==r) {root[now]=a[rnum[l]];return;}
    int mid=(l+r)/2;
    bulidtree(l,mid,2*now);
    bulidtree(mid+1,r,2*now+1);
    pushup(now);
}
void changetree(int l,int r,int L,int R,int now,int cn){
    if(L>r||R<l) return;
    if(L>=l&&R<=r) {root[now]+=cn*(R-L+1);lazy[now]+=cn;return;}
    pushdown(now,R-L+1);
    int mid=(L+R)/2;
    changetree(l,r,L,mid,now*2,cn);
    changetree(l,r,mid+1,R,now*2+1,cn);   
    pushup(now);
}
long long query(int l,int r,int L,int R,int now){
    if(L>r||R<l) return 0;
    if(L>=l&&R<=r) return root[now];
    int mid=(L+R)/2;
    pushdown(now,R-L+1);
    return query(l,r,L,mid,now*2)+query(l,r,mid+1,R,now*2+1);
}
void lca(int x,int y,int cn){
    while(top[x]!=top[y]){
        if(d[top[x]]>d[top[y]]){
            changetree(num[top[x]],num[x],1,n,1,cn);
            x=fa[top[x]];
        }
        else {
            changetree(num[top[y]],num[y],1,n,1,cn);
            y=fa[top[y]];
        }
    }
    if(d[x]<d[y]) changetree(num[x],num[y],1,n,1,cn);
    else changetree(num[y],num[x],1,n,1,cn);
}
int main(void){
    int m,p;
    while(scanf("%d%d%d",&n,&m,&p)==3){
        for(int i=1;i<=n;i++){scanf("%d",&a[i]);line[i].clear();}
        int x,y,z;
        for(int i=1;i<=m;i++){
            scanf("%d%d",&x,&y);
            line[x].push_back(y);
            line[y].push_back(x);
        }
        cnt=0;
        d[0]=0;
        size[0]=0;
        dfs1(0,1);
        dfs2(1,1);
        bulidtree(1,n,1);
        char q;
        for(int i=1;i<=p;i++){
            q=getchar();
            while(q==' '||q=='\n') q=getchar();
            if(q=='Q') {scanf("%d",&x);printf("%lld\n",query(num[x],num[x],1,n,1));}
            else {
                scanf("%d%d%d",&x,&y,&z);
                if(q=='D') z=-z; 
                lca(x,y,z);
            }
        }
    }
}