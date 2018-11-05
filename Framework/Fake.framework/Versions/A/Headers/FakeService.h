//
//  FakeService.h
//  Fake
//
//  Created by Hays on 2018/11/5.
//  Copyright © 2018 Hays. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@protocol FakeServiceDelegate <NSObject>

- (void)onFrontConnected;
- (void)onCountUpdate:(NSInteger)count;
- (void)onFrontDisConnected;

@end

@interface FakeService : NSObject

@property(nonatomic, weak) id<FakeServiceDelegate> delegate;

- (void)test1;

- (NSInteger)test2;

- (void)test3:(NSString *)text;

- (void)test4:(NSInteger)arg1 text:(NSString *)text isTrue:(BOOL)isTrue;

- (void)asyncTest:(dispatch_block_t)callback;

- (void)addSubView:(NSView *)view;

@end

NS_ASSUME_NONNULL_END
