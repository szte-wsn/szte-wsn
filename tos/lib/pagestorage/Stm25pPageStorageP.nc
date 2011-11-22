
#include "StorageVolumes.h"
#include "Stm25p.h"

module Stm25pPageStorageP {
  provides interface PageStorage[uint8_t id];
  //provides interface Stm25pVolume as Volume[ uint8_t id ];
  uses interface Stm25pSector as Sector[ uint8_t id ];
  uses interface Resource as ClientResource[ uint8_t id ];
  uses interface Leds;
  uses interface DiagMsg;
}

implementation{
  enum{
    S_IDLE,
    S_READ,
    S_WRITE,
    S_ERASE,
    S_REAL_ERASE,
  };
  enum{
    N=uniqueCount("Stm25p.Page"),
  };
  
  typedef struct client_data{
    uint8_t status;
    uint32_t pageNum;//page for read/write sector for erase
    uint8_t* buffer;
  } client_data;
  
  client_data clients[N];
  
  command error_t PageStorage.read[uint8_t id](uint32_t pageNum, void *buffer){
    if(pageNum >= (uint32_t)(call Sector.getNumSectors[id]() << (STM25P_SECTOR_SIZE_LOG2-STM25P_PAGE_SIZE_LOG2)))
      return EINVAL;
    if(clients[id].status==S_READ)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    
    clients[id].status=S_READ;
    clients[id].pageNum=pageNum;
    clients[id].buffer=buffer;
    return call ClientResource.request[id]();
  }
  command error_t PageStorage.write[uint8_t id](uint32_t pageNum, void *buffer){
    if(pageNum >= (uint32_t)(call Sector.getNumSectors[id]() << (STM25P_SECTOR_SIZE_LOG2-STM25P_PAGE_SIZE_LOG2)))
      return EINVAL;
    if(clients[id].status==S_WRITE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    
    clients[id].status=S_WRITE;
    clients[id].pageNum=pageNum;
    clients[id].buffer=buffer;
    return call ClientResource.request[id]();
  }
  
  command error_t PageStorage.erase[uint8_t id](uint32_t sectorNum, bool realErase){
    if(sectorNum >= call Sector.getNumSectors[id]())
      return EINVAL;
    if(clients[id].status==S_ERASE)
      return EALREADY;
    else if(clients[id].status!=S_IDLE)
      return EBUSY;
    
    clients[id].status=S_ERASE;
    clients[id].pageNum=sectorNum;
    return call ClientResource.request[id]();
  }
  
  inline void signalDone(uint8_t id, uint8_t status, error_t error){
    clients[id].status=S_IDLE;
    call ClientResource.release[id]();
    switch(status){
      case S_READ:{
        signal PageStorage.readDone[id]( clients[id].pageNum, clients[id].buffer, error );
      }break;
      case S_WRITE:{
        signal PageStorage.writeDone[id]( clients[id].pageNum, clients[id].buffer, error );
      }break;
      case S_ERASE:{
        signal PageStorage.eraseDone[id]( clients[id].pageNum, TRUE, error );
      }break;
    }
  }
  
  event void ClientResource.granted[uint8_t id](){
    error_t lastError=SUCCESS;
    switch(clients[id].status){
      case S_READ:{
        lastError=call Sector.read[id]( clients[id].pageNum<<STM25P_PAGE_SIZE_LOG2, clients[id].buffer, STM25P_PAGE_SIZE );
      }break;
      case S_WRITE:{
        lastError=call Sector.write[id]( clients[id].pageNum<<STM25P_PAGE_SIZE_LOG2, clients[id].buffer, STM25P_PAGE_SIZE );
      }break;
      case S_ERASE:{
        lastError=call Sector.erase[id]( clients[id].pageNum, 1 );
      }break;
    }
    if(lastError != SUCCESS){
      signalDone(id, clients[id].status, lastError);
    }
  }
  
  event void Sector.writeDone[uint8_t id]( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len, error_t error ){
      signalDone(id, S_WRITE, error);
  }
  
  event void Sector.eraseDone[uint8_t id]( uint8_t sector, uint8_t num_sectors, error_t error ){
      signalDone(id, S_ERASE, error);
  }
  
  event void Sector.readDone[uint8_t id]( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len, error_t error ){
      signalDone(id, S_READ, error);
  }
  
  event void Sector.computeCrcDone[uint8_t id]( stm25p_addr_t addr, stm25p_len_t len, uint16_t crc, error_t error ){}

  default command storage_addr_t Sector.getPhysicalAddress[ uint8_t id ]( storage_addr_t addr ) { return 0xffffffff; }
  default command uint8_t Sector.getNumSectors[ uint8_t id ]() { return 0; }
  default command error_t Sector.read[ uint8_t id ]( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len ) { return FAIL; }
  default command error_t Sector.write[ uint8_t id ]( stm25p_addr_t addr, uint8_t* buf, stm25p_len_t len ) { return FAIL; }
  default command error_t Sector.erase[ uint8_t id ]( uint8_t sector, uint8_t num_sectors ) { return FAIL; }
  default command error_t Sector.computeCrc[ uint8_t id ]( uint16_t crc, storage_addr_t addr, storage_len_t len ) { return FAIL; }
  
  default async command error_t ClientResource.request[ uint8_t id ]() { return FAIL; }
  default async command error_t ClientResource.release[ uint8_t id ]() { return FAIL; }
  
  default event void PageStorage.readDone[uint8_t id](uint32_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.writeDone[uint8_t id](uint32_t pageNum, void *buffer, error_t error){}
  default event void PageStorage.eraseDone[uint8_t id](uint32_t sectorNum, bool realErase, error_t error){}
}