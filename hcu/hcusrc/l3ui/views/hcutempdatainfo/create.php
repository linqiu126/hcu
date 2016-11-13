<?php

use yii\helpers\Html;


/* @var $this yii\web\View */
/* @var $model app\models\Hcutempdatainfo */

$this->title = 'Create Hcutempdatainfo';
$this->params['breadcrumbs'][] = ['label' => 'Hcutempdatainfos', 'url' => ['index']];
$this->params['breadcrumbs'][] = $this->title;
?>
<div class="hcutempdatainfo-create">

    <h1><?= Html::encode($this->title) ?></h1>

    <?= $this->render('_form', [
        'model' => $model,
    ]) ?>

</div>
