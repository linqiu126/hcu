<?php

use yii\helpers\Html;
use yii\widgets\ActiveForm;

/* @var $this yii\web\View */
/* @var $model app\models\HcusyspmglobaldatainfoSearch */
/* @var $form yii\widgets\ActiveForm */
?>

<div class="hcusyspmglobaldatainfo-search">

    <?php $form = ActiveForm::begin([
        'action' => ['index'],
        'method' => 'get',
    ]); ?>

    <?= $form->field($model, 'sid') ?>

    <?= $form->field($model, 'taskhcuvmec') ?>

    <?= $form->field($model, 'taskhwinvec') ?>

    <?= $form->field($model, 'tasksyspmec') ?>

    <?= $form->field($model, 'taskmodbusec') ?>

    <?php // echo $form->field($model, 'taskcloudvelaec') ?>

    <?php // echo $form->field($model, 'taskavorionec') ?>

    <?php // echo $form->field($model, 'taskspsvirgoec') ?>

    <?php // echo $form->field($model, 'taskhsmmpec') ?>

    <?php // echo $form->field($model, 'taskemcec') ?>

    <?php // echo $form->field($model, 'taskpm25ec') ?>

    <?php // echo $form->field($model, 'taskwinddirec') ?>

    <?php // echo $form->field($model, 'taskwindspdec') ?>

    <?php // echo $form->field($model, 'tasktempec') ?>

    <?php // echo $form->field($model, 'taskhumidec') ?>

    <?php // echo $form->field($model, 'tasknoiseec') ?>

    <?php // echo $form->field($model, 'taskairprsec') ?>

    <?php // echo $form->field($model, 'taskco1ec') ?>

    <?php // echo $form->field($model, 'tasklightstrec') ?>

    <?php // echo $form->field($model, 'taskalcoholec') ?>

    <?php // echo $form->field($model, 'taskhchoec') ?>

    <?php // echo $form->field($model, 'tasktoxicgasec') ?>

    <?php // echo $form->field($model, 'restartcnt') ?>

    <?php // echo $form->field($model, 'cloudvelaconncnt') ?>

    <?php // echo $form->field($model, 'cloudveladisccnt') ?>

    <?php // echo $form->field($model, 'clouddatatimeoutcnt') ?>

    <?php // echo $form->field($model, 'timestamp') ?>

    <div class="form-group">
        <?= Html::submitButton('Search', ['class' => 'btn btn-primary']) ?>
        <?= Html::resetButton('Reset', ['class' => 'btn btn-default']) ?>
    </div>

    <?php ActiveForm::end(); ?>

</div>
